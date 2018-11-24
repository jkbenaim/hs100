#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "version.h"
#include "comms.h"

struct cmd_s {
	char *command;
	char *help;
	char *json;
	char *(*handler)(int argc, char *argv[]);
	int end;
};
struct cmd_s cmds[] = {
	{
		.command = "factory-reset",
		.help = "factory-reset\treset the plug to factory settings",
		.json = "{\"system\":{\"reset\":{\"delay\":0}}}",
	},
	{
		.command = "off",
		.help = "off\t\tturn the plug on",
		.json = "{\"system\":{\"set_relay_state\":{\"state\":0}}}",
	},
	{
		.command = "on",
		.help = "on\t\tturn the plug off",
		.json = "{\"system\":{\"set_relay_state\":{\"state\":1}}}",
	},
	{
		.command = "reboot",
		.help = "reboot\t\treboot the plug",
		.json = "{\"system\":{\"reboot\":{\"delay\":0}}}",
	},
	{
		.end = 1,
	},
};

struct cmd_s *get_cmd_from_name(char *needle)
{
	int cmds_index = 0;
	while(!cmds[cmds_index].end)
	{
		if(!strcmp(cmds[cmds_index].command, needle))
			return &cmds[cmds_index];
		cmds_index++;
	}
	return NULL;
}

void print_usage()
{
	fprintf(stderr, "hs100 version " VERSION_STRING ", Copyright (C) 2018 Jason Benaim.\n"
			"A tool for using certain wifi smart plugs.\n"
			"\n"
			"usage: hs100 <ip> <command>\n"
			"\n"
			"Commands:\n"
	);
	int cmds_index = 0;
	while(!cmds[cmds_index].end)
	{
		fprintf(stderr, "\t%s\n", cmds[cmds_index].help);
		cmds_index++;
	}
	fprintf(stderr, "\n"
			"Report bugs to https://github.com/jkbenaim/hs100\n"
	);
}

int main(int argc, char *argv[])
{
	if(argc != 3) {
		print_usage();
		return 1;
	}
	char *plug_addr = argv[1];
	char *cmd_string = argv[2];
	char *response;

	struct cmd_s *cmd = get_cmd_from_name(cmd_string);
	if(cmd != NULL) {
		if(cmd->handler != NULL)
			response = cmd->handler(argc, argv);
		else
			response = hs100_send(plug_addr, cmd->json);
	} else {
		// command not recognized, so send it to the plug raw
		response = hs100_send(plug_addr, cmd_string);
	}
	
	if(response == NULL) {
		fprintf(stderr, "failed to send command\n");
		return 1;
	}

	printf("%s\n", response);

	free(response);
	return 0;
}
