#include <stdio.h>
#include "../includes/minishell.h"

void	init_abort(void)
{
	g_shell->abort = 0;
	g_shell->abort_dred = 0;
	g_shell->in_dred = 0;
}

void	init_g_shell(char **envp)
{
	g_shell = ft_calloc(1, sizeof(t_shell));
	ft_memset((g_shell), 0, sizeof(t_shell));
	(g_shell)->current = NULL;
	ft_new_history(&(g_shell)->current);
	(g_shell)->current->index = 0;
	(g_shell)->n_up = 0;
	g_shell->prompt = ft_calloc(0, 1);
	g_shell->env = init_env(envp);
	init_abort();
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &g_shell->w);
	edit_env(&g_shell->env, "OLDPWD", ft_getenv(g_shell->env, "PWD"));
}

void	garbage_collect(t_cmd **cmd_arr, t_cmd *cmd)
{
	int		arr_i;
	t_cmd	*tmp;

	arr_i = 0;
	while (*(cmd_arr + arr_i))
	{
		cmd = *(cmd_arr + arr_i);
		while (cmd)
		{
			ft_free_matrix(cmd->arr);
			if (cmd->file_in != 0)
				close(cmd->file_in);
			if (cmd->file_out != 1)
				close(cmd->file_out);
			free(cmd->eof);
			tmp = cmd;
			cmd = cmd->next;
			free(tmp);
		}
		arr_i++;
	}
	free(cmd_arr);
}

void	shell(t_cmd **cmd_arr, t_cmd *cmd, int arr_i)
{
	g_shell->pid = 0;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &g_shell->w);
	set_prompt("\e[0;32mCON\033[0;37mCHIG\033[0;31mLIA -> \e[0m🤌  ");
	if (!g_shell->abort_dred)
		get_prompt();
	ft_hook_signal();
	if (!g_shell->abort_dred)
	{
		init_abort();
		hook_line(g_shell);
	}
	init_abort();
	if (ft_strlen(g_shell->current->row) && !only_spaces(g_shell->current->row))
	{
		cmd_arr = start_parsing(g_shell->current->row);
		arr_i = 0;
		while (*(cmd_arr + arr_i))
		{
			cmd = *(cmd_arr + arr_i);
			if (!cmd->ignore && cmd->len)
				ft_executor(cmd, g_shell->env);
			arr_i++;
		}
		garbage_collect(cmd_arr, cmd);
	}
}

int	main(int n, char **arg, char **envp)
{
	t_cmd	**cmd_arr;
	t_cmd	*cmd;
	int		arr_i;

	(void)n;
	(void)arg;
	arr_i = 0;
	cmd_arr = 0;
	cmd = 0;
	init_g_shell(envp);
	if (!(n > 1 && !ft_strcmp("-now", arg[1])))
		watermark();
	write(1, "\r\033[2K", 5);
	while (1)
	{
		shell(cmd_arr, cmd, arr_i);
		if (ft_strlen(g_shell->current->row) && !g_shell->abort_dred)
			ft_new_history(&g_shell->current);
	}
	return (0);
}
