/**
 * @file rini.c
 * @author LloydLabs
 * @date 10/2/2017
 * @brief Rini is a tiny, super fast, .ini file parser programmed from scratch in C99.
 * Rini (really small ini parser).
 * All other parsers rely on libc functions such as 'strtok', however Rini is in complete raw C code designed to handle the 'ini' format thus is much faster and efficent.
 * This is the first parser I've made completely from scratch, feedback is welcome.
 *
 * @see https://github.com/LloydLabs/librini
 * @see https://en.wikipedia.org/wiki/INI_file
 */

#include "rini.h"
#include "lib.h"

/**
 * seeks to the location of the start of the [parent]
 * @param parent The name of the parent
 * @param config_buf The configuration
 * @param size The size of the confuration
 * @return Returns an address to where the [parent] starts
 */
static char* rini_seek_section(const char* parent, char* config_buf, unsigned size)
{
    unsigned buf_size = 0;
    int head_found = 0;

    char current_head[MAX_NAME];

#ifdef __STDC_HOSTED__
    rini_memset(current_head, 0, MAX_NAME);
#else
    memset(current_head, 0, MAX_NAME);
#endif

    char* head_buf = (char*)current_head;

    for (unsigned total_read = 0; total_read < size; total_read++, config_buf++)
    {
        if (*config_buf == '[')
        {
            if (total_read++ >= size)
            {
                break;
            }

            config_buf++;

            for ( ; *config_buf != ']' && total_read < size; config_buf++, buf_size++, total_read++)
            {
                if ((buf_size + 1) >= MAX_NAME)
                {
                    break;
                }

                *head_buf++ = *config_buf;
            }

            if (total_read++ >= size)
            {
                break;
            }

            config_buf++;

            if (
#ifdef __STDC_HOSTED__
                    rini_strncmp(parent, (const char*)current_head, MAX_NAME) == 0
#else
                    strncmp(parent, (const char*)current_head, MAX_NAME) == 0
#endif
                    )
            {
                head_found = 1;
                break;
            }
            else
            {
                CLEAN_PTR(current_head, sizeof(current_head), head_buf);
            }
        }
    }

    return (head_found ? config_buf : NULL);
}

/**
 * converts a string into a signed integer
 * @param str The string to convert.
 * @param flags Flags passed by the parser, used to specifying if negative.
 * @return The converted integer.
 */
static int rini_signed_int_str(char* str, parser_flags_t* flags)
{
    int ret = -1;

    *str++ = 0;
    str = (char*)str;

    if (*str == '+' || *str == '-')
    {
        str++;
        if (*str == '-')
        {
            *flags |= INT_NEG_NUMB;
        }
    }
    else if (*str == '-')
    {
        *flags |= INT_NEG_NUMB;
        str++;
    }

    for ( ; *str != 0; str++)
    {
        ret = ((10 * ret) - (*str - '0'));
    }

    ret *= -1;

    if (*flags & INT_NEG_NUMB)
    {
        ret = (~ret) + 1;
    }

    return ret;
}

/**
 * https://en.wikipedia.org/wiki/INI_file#Escape_characters
 * checks if a character is of a special type and must be escaped
 * @param c The character to check
 * @return This will return 1 on success
 */
static int rini_is_escaped(char c)
{
    // https://en.wikipedia.org/wiki/INI_file#Escape_characters
    const char escape_chars[] = {
            '"', ';', '#', ':',
            '=', '\\'
    };

    for (int i = 0; i < ARRAY_SIZE(escape_chars); i++)
    {
        if (c == escape_chars[i])
        {
            return 1;
        }
    }

    return 0;
}

/**
 * attempts to parse a string of a given size from a human-like boolean value.
 * @see bool_type_t
 * @param buf The alleged boolean value.
 * @param buf_size The size of the buffer.
 * @return Will return BOOL_KEY_ERROR on error, else, a boolean value.
 */
static bool_type_t rini_get_bool(char* buf, unsigned buf_size)
{
    if (buf == NULL || buf_size > MAX_BOOL_KEY_SIZE)
        return BOOL_KEY_ERROR;

    bool_keys_t bool_keys[] = {
            { BOOL_KEY_TRUE, "1", 1 }, { BOOL_KEY_TRUE, "yes", 3 },
            { BOOL_KEY_TRUE, "on", 2 }, { BOOL_KEY_TRUE, "true", 4 },
            { BOOL_KEY_FALSE, "0", 1 },  { BOOL_KEY_FALSE, "no", 2 },
            { BOOL_KEY_FALSE, "off", 3 }, { BOOL_KEY_FALSE, "false", 5 }
    };

    char bool_key[MAX_BOOL_KEY_SIZE];

#ifdef __STDC_HOSTED__
    rini_memset(bool_key, 0, MAX_BOOL_KEY_SIZE);
    rini_memcpy(bool_key, buf, buf_size);
#else
    memset(bool_key, 0, MAX_BOOL_KEY_SIZE);
    memcpy(bool_key, buf, buf_size);
#endif

    for (unsigned i = 0; i < ARRAY_SIZE(bool_keys); i++)
    {
        if (bool_keys[i].size > buf_size)
        {
            continue;
        }

        if (
#ifdef __STDC_HOSTED__
            rini_memcmp(bool_key, bool_keys[i].key, bool_keys[i].size) == 0
#else
            memcmp(bool_key, bool_keys[i].key, bool_keys[i].size) == 0
#endif
            )
        {
            return bool_keys[i].val;
        }
    }

    return BOOL_KEY_ERROR;
}

/**
 * The main parsing logic is conducted here, first of all the key is parsed from the input,
 * we then make sure the key is valid. Then, from here, we skip the '=' character, and then go on to
 * parse the value. Once the value has been parsed, we do the relevant operations to convert it into
 * the data type specified in the @param val_type.
 *
 * @param node Usually the line which is to be parsed.
 * @param name The name to the key to parse.
 * @param out The buffer to send the output to.
 * @param out_size The size of the output buffer.
 * @param val_type The type of value that we are parsing.
 * @param size The overall size of the configuration
 * @return Returns 1 on success.
 */
static int rini_get_node(char* node, char* name, void* out, unsigned out_size, value_types_t val_type, unsigned size)
{
    if (out == NULL)
    {
        return 0;
    }

    if (val_type == INT_VAL && out_size < sizeof(int))
    {
        return 0;
    }

    char name_parsed[MAX_NAME], int_str[MAX_INT_STR_SIZE], bool_str[MAX_BOOL_KEY_SIZE];

#ifdef __STDC_HOSTED__
    rini_memset(name_parsed, 0, MAX_NAME);
    rini_memset(int_str, 0, MAX_INT_STR_SIZE);
#else
    memset(name_parsed, 0, MAX_NAME);
    memset(int_str, 0, MAX_INT_STR_SIZE);
#endif

    char* name_buf = (char*)name_parsed, *int_buf = (char*)int_str, *bool_buf = (char*)bool_str, *node_buf = node;
    unsigned buf_size = 0, val_size = 1;

    for ( ; PTR_NOT_END(node_buf) && buf_size < size; node_buf++, buf_size++)
    {
        for ( ; *node_buf != '=' && buf_size < size; node_buf++, buf_size++)
        {
            if ((buf_size + 1) >= MAX_NAME)
            {
                return 0;
            }

            *name_buf++ = *node_buf;
        }

        if (
#ifdef __STDC_HOSTED__
            rini_strncmp(name_parsed, name, MAX_NAME) == 0
#else
            strncmp(name_parsed, name, MAX_NAME) == 0
#endif
            )
        {
            break;
        }
    }

    if (*node_buf != '=')
    {
        return 0;
    }

    if (buf_size++ > size)
    {
        return 0;
    }

    node_buf++;

    parser_flags_t parser_flags = FLAGS_DEFAULT;

    char* val_buf = (char*)out;
    for ( ; PTR_NOT_END(node_buf) && buf_size < size; node_buf++, buf_size++, val_size++)
    {
        if (parser_flags & END_EARLY)
        {
            break;
        }

        if ((parser_flags & LAST_ESCAPE_CHAR) == 0)
        {
            if (*node_buf == '#' || *node_buf == ';')
            {
                parser_flags |= END_EARLY;
                continue;
            }
        }

        switch (val_type)
        {
            case STRING_VAL:
                if ((val_size + 1) > out_size)
                {
                    return 0;
                }

                if (buf_size == 0)
                {
                    if (*node_buf == '"')
                    {
                        parser_flags |= EXPECT_QUOTE;
                        continue;
                    }
                }

                if (*node_buf == '\\')
                {
                    parser_flags |= LAST_ESCAPE_CHAR;
                    break;
                }

                if (parser_flags & LAST_ESCAPE_CHAR)
                {
                    parser_flags |= ESCAPE_NOT_FOUND;
                    if (rini_is_escaped(*node_buf))
                    {
                        *val_buf++ = *node_buf;
                        parser_flags &= ~ESCAPE_NOT_FOUND;
                    }

                    if (parser_flags & ESCAPE_NOT_FOUND)
                    {
                        *val_buf++ = '\\';
                        node_buf--;
                        buf_size--;

                        parser_flags &= ~ESCAPE_NOT_FOUND;
                        continue;
                    }

                    parser_flags &= ~LAST_ESCAPE_CHAR;
                }
                else if (*node_buf == '"' && (parser_flags & EXPECT_QUOTE))
                {
                    parser_flags |= END_EARLY;
                    continue;
                }
                else
                {
                    *val_buf++ = *node_buf;
                }
                break;

            case BOOL_VAL:
                if (val_size > (MAX_BOOL_KEY_SIZE - 1))
                {
                    return 0;
                }

                *bool_buf++ = *node_buf;
                break;

            case INT_VAL:
                if (val_size > (MAX_INT_STR_SIZE - 1))
                {
                    return 0;
                }

                if (buf_size == 0 && *node_buf == '-')
                {
                    *int_buf++ = *node_buf;
                    continue;
                }

                if (*node_buf < '0' || *node_buf > '9')
                {
                    return 0;
                }

                *int_buf++ = *node_buf;
                break;
        }
    }

    if (val_type == STRING_VAL)
    {
        *val_buf++ = 0;
    }
    else if (val_type == INT_VAL)
    {
        *int_buf++ = 0;
        int_buf = (char*)int_str;

        int numb_buf;
        if ((numb_buf = rini_signed_int_str(int_buf, &parser_flags)) < 0)
        {
            return 0;
        }

#ifdef __STDC_HOSTED__
        rini_memcpy(out, &numb_buf, sizeof(int));
#else
        memcpy(out, &numb_buf, sizeof(int));
#endif
    }
    else if (val_type == BOOL_VAL)
    {
        bool_type_t bool_val = BOOL_KEY_ERROR;
        if ((bool_val = rini_get_bool(bool_str, val_size)) == BOOL_KEY_ERROR)
        {
            return 0;
        }

        *val_buf = bool_val;
    }

    return 1;
}

/**
 * this function will parse a key from the given parent, key and configuration.
 * @see rini_seek_section
 * @see rini_get_node
 * @param parent The parent [section], NULL if no section.
 * @param key The name of the key to find
 * @param config The configuration to parse
 * @param config_size The size of the configuration
 * @param out The buffer to recieve the key's value.
 * @param out_size The size of the buffer to recieve the key's value.
 * @param type The type of data that the key holds.
 * @return On success 1 is returned
 */
int rini_get_key(const char* parent, char* key, char* config, unsigned config_size, const void* out, unsigned out_size, value_types_t type)
{
    char* config_buf = config;

    if (key != NULL)
    {
        if ((config_buf = rini_seek_section(parent, config_buf, config_size)) == NULL)
        {
            return 0;
        }
    }

    char line[MAX_LINE_SIZE(out_size)];

#ifdef __STDC_HOSTED__
    rini_memset(line, 0, MAX_NAME);
#else
    memset(line, 0, MAX_NAME);
#endif

    char* line_buf = (char*)line;
    unsigned line_size = 0;

    for (unsigned total_read = 0; total_read < config_size; config_buf++, total_read++)
    {
        line_size = 0;
        for ( ; PTR_NOT_END(config_buf) && total_read < config_size; config_buf++, line_size++, total_read++)
        {
            if ((line_size + 1) >= MAX_LINE_SIZE(out_size))
            {
                return 0;
            }

            if (line_size == 0)
            {
                if (*config_buf == '#' || *config_buf == ';')
                {
                    continue;
                }

                if (*config_buf == '[')
                {
                    break;
                }
            }

            *line_buf++ = *config_buf;
        }

        if (rini_get_node(line, key, (void*)out, out_size, type, line_size) == 1)
        {
            return 1;
        }

        CLEAN_PTR(line, MAX_NAME, line_buf);
    }

    return 0;
}
