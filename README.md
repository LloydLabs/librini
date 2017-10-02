[![MIT Licence](https://badges.frapsoft.com/os/mit/mit.svg?v=103)](https://opensource.org/licenses/mit-license.php)

# Rini - librini
Rini (*really small ini parser*) is a tiny, standalone, blazing fast .ini file parser. All other parsers rely on libc functions such as 'strtok', however Rini is in complete raw C code designed to handle the 'ini' format thus is much faster and efficent. This is the first parser I've made completely from scratch, feedback is welcome. I'll be removing the libc helper functions in the next release.

# Advantages over other parsers
- Does not rely on any standard library functions. No use of strstr, strcmp, strtok etc.
- Much smaller in size compared to other parsers.
- Clean, maintainable code.
- Much faster than its counterparts.
- Tiny footprint. Great for an embedded environment.
- Does not touch the heap at all, meaning less overhead of useless allocations. It is stack based only.
- Support for inline comments and the escaping of special characters.

# Supported data types
- Strings, ASCII strings - no support for UTF-8 currently.
- Booleans, tokens such as 'TRUE', '1', 'ON', 'YES' are all accepted (and all of their lowercase equivalents).
- Signed integers, Rini has a built-in signed integer converter.

`MAX_NAME` is the set maximum length of keys (not values) and headers and has a default value of 256, you may however change this macro in `rini.h`.

# Building
```
cmake --build .
```

# Usage

Consider we have the following INI format:
```ini
[main]
# Rini is so fast!
name=Lloyd
age=18 ; librini supports inline comments
programmer=yes
[server.labs]
port=123
password=github
```

```C
const char config[] = "[main]"
                      "# Consider this is a comment"
                      "name=Lloyd\n"
                      "age=18 ; librini supports inline comments\n"
                      "programmer=yes\n"
                      "[server.labs]\n"
                      "port=123\n"
                      "password=github\n";

char val_name[MAX_KEY];
if (rini_get_key("main", "name", (char*)config, sizeof(config), val_name, sizeof(val_name), STRING_VAL))
{
  printf("Name: %s\n", val_name);
}

int is_programmer = 0;
if (rini_get_key("main", "programmer", (char*)config, sizeof(config), &is_programmer, sizeof(is_programmer), BOOL_VAL))
{
  printf("Programmer status [1/0]: %d\n", is_programmer);
}

int server_port = 0;
if (rini_get_key("server.labs", "port", (char*)config, sizeof(config), &server_port, sizeof(server_port), INT_VAL))
{
  printf("Binding to port.. %d\n", server_port);
}

```
