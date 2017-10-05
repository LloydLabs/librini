#include <stdio.h>
#include <stdlib.h>

#include <rini.h>

// Globals are bad, this is just here for the example, please don't use them.
const char config[] = "[server.main]"
                    "# This server is used for clustering"
                    "hostname=root\n"
                    "distro=Ubuntu\n"
                    "ssh_port=99\n"
                    "[server.labs]\n"
                    "hostname=honeypot\n"
                    "distro=Debian\n"
                    "ssh_port=80\n";

int main(int argc, char** argv)
{
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

  return 0;
}
