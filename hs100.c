#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "comms.h"

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
