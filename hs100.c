#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "comms.h"

const char usage[] = 
"usage: hs100 ip command-or-json\n"
"where 'ip' is an IP address (ex. 192.168.1.1)\n"
"and 'command' is one of the words 'on', 'off', or a blob of json\n";

struct cmd_alias_s {
	char *alias;
	char *command;
	int end;
};
struct cmd_alias_s cmd_aliases[] = {
	{
		.alias = "off",
		.command = "{\"system\":{\"set_relay_state\":{\"state\":0}}}",
	},
	{
		.alias = "on",
		.command = "{\"system\":{\"set_relay_state\":{\"state\":1}}}",
	},
	{
		.alias = "reboot",
		.command = "{\"system\":{\"reboot\":{\"delay\":0}}}",
	},
	{
		.alias = "reset-yes-really",
		.command = "{\"system\":{\"reset\":{\"delay\":0}}}",
	},
	{
		.end = 1,
	},
};

char *get_cmd(char *needle)
{
	int cmds_index = 0;
	while(!cmd_aliases[cmds_index].end)
	{
		if(!strcmp(cmd_aliases[cmds_index].alias, needle))
			return cmd_aliases[cmds_index].command;
		cmds_index++;
	}
	return needle;
}

int main(int argc, char *argv[])
{
	if(argc != 3) {
		fprintf(stderr, "%s", usage);
		return 1;
	}
	char *plug_addr = argv[1];
	char *cmd = argv[2];

	cmd = get_cmd(cmd);
	
	char *response = hs100_send(plug_addr, cmd);
	
	if(response == NULL) {
		fprintf(stderr, "failed to send command\n");
		return 1;
	}

	printf("%s\n", response);

	free(response);
	return 0;
}
