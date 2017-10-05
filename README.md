[![MIT Licence](https://badges.frapsoft.com/os/mit/mit.svg?v=103)](https://opensource.org/licenses/mit-license.php)

# Rini - librini
Rini (*really small ini parser*) is a tiny, standalone, blazing fast .ini file parser. All other parsers rely on libc functions such as 'strtok', however Rini is in complete raw C code designed to handle the 'ini' format thus is much faster and efficient. This is the first parser I've made completely from scratch, feedback is welcome. I'll be removing the libc helper functions in the next release.

# Advantages over other parsers
- No overhead of using functions such as strtok.
- Much smaller in size compared to other parsers.
- Clean, maintainable code.
- Much faster than its counterparts.
- Tiny footprint. Great for an embedded environment.
- Does not touch the heap at all, meaning less overhead of useless allocations. It is stack based only.
- Support for inline comments and the escaping of special characters.

# Supported data types
- Strings, ASCII strings - no support for wide characters currently. Rini supports "quoted strings" too. When it comes to strings too, Rini supports all escaped special characters. For example, to escape the ';' character which is used for inline comments: "$x = 0\;".
- Booleans, tokens such as 'TRUE', '1', 'ON', 'YES', 'Y' are all accepted (and all of their lowercase equivalents).
- Signed integers, Rini has a built-in signed integer converter.

`MAX_NAME` is the set maximum length of keys (not values) and headers and has a default value of 256, you may however change this macro in `rini.h`.

# Building
```
cmake --build .
```

# Usage

The only method exported by librini is `rini_get_key` which is located in the API header `rini.h`.

```C
int rini_get_key(const char* parent, const char* key, const char* config, unsigned config_size, const void* out, unsigned out_size, value_types_t type)
```

Consider we have the following INI format in a variable named `config` in the type `const char`:
```ini
[server.main]
# This server is used for clustering..
hostname="root"
distro=Ubuntu
ssh_port=99
[server.labs]
hostname=honeypot
distro=Debian
ssh_port=80 ; this is the SSH port
```

```C
char server_hostname[MAX_NAME], distro_name[MAX_NAME];
memset(server_hostname, 0, sizeof(server_hostname));
memset(distro_name, 0, sizeof(distro_name));

int main_port = 0;

if (rini_get_key("server.main", "hostname", config, sizeof(config), server_hostname, sizeof(server_hostname), STRING_VAL))
{
  printf("Name of main server: %s\n", server_hostname);
}

if (rini_get_key("server.main", "ssh_port", config, sizeof(config), &main_port, sizeof(main_port), INT_VAL))
{
  if (main_port <= 1024)
  {
    printf("Please change the port from %d to a number above 1024\n", main_port);
  }
}

if (rini_get_key("server.labs", "distro", config, sizeof(config), distro_name, sizeof(distro_name), STRING_VAL))
{
  if (strncmp(distro_name, "Arch", sizeof(distro_name)) == 0)
  {
    puts("You have a proper distro installed");
  }
}
```
