#include "cub3D.h"

void	init_image(t_data *data)
{
	int bpp, size_line, endian;
	data->img = mlx_new_image(data->mlx, data->width * 50, data->height * 50);
	data->addr = mlx_get_data_addr(data->img, &bpp, &size_line, &endian);
	data->bits_per_pixel = bpp;
	data->line_length = size_line;
	data->endian = endian;
}

void	my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

int	close_window(t_data *data)
{
	mlx_destroy_image(data->mlx, data->img);
	mlx_destroy_window(data->mlx, data->win);
	// Free map memory
	for (int i = 0; i < data->height; i++)
		free(data->map[i]);
	free(data->map);
	exit(0);
}

void update_game(t_data *data)
{
    if (data->update_needed)
    {
        // Update player position based on walk_dir
        data->player.x += cos(data->player.rotation_angle) * data->player.moves_speed * data->player.walk_dir;
        data->player.y += sin(data->player.rotation_angle) * data->player.moves_speed * data->player.walk_dir;

        // Update player rotation based on turn_dir
        data->player.rotation_angle += data->player.turn_dir * data->player.rotation_speed;
        data->update_needed = 0;
    }
}
int	key_press(int keycode, t_data *data)
{
	printf("my data is %d\n", keycode);
	if (keycode == 65307)
		close_window(data);
	else if (keycode == 100 || keycode == 65363)
	{
        data->player.turn_dir = 1;
	}
	else if (keycode == 97 || keycode == 65361)
	{
         data->player.turn_dir = -1;
	}
	else if (keycode == 115 || keycode == 65364)
    {
        data->player.walk_dir = -1;
    }
	else if (keycode == 119 || keycode == 65362)
    {
        data->player.walk_dir = 1;
    }
    data->update_needed = 1;
     update_game(data);
	return (keycode);
}
int key_release(int keycode, t_data *data)
{
    if (keycode == 100 || keycode == 65363 || keycode == 97 || keycode == 65361)
        data->player.turn_dir = 0;
    else if (keycode == 115 || keycode == 65364 || keycode == 119 || keycode == 65362)
        data->player.walk_dir = 0;

    return (0);
}
void draw_line(t_data *data, int center_x, int center_y, double length, int color)
{
    double dx = cos(data->player.rotation_angle) * length;
    double dy = sin(data->player.rotation_angle) * length;
    
    for (double i = 0; i <= length; i++)
    {
        double x = center_x + (double)(dx * i / length);
        double y = center_y + (double)(dy * i / length);
        my_mlx_pixel_put(data, x, y, color);
    }
}
void draw_circle(t_data *data, int center_x, int center_y, int radius, int color)
{
     for (int y = -radius; y <= radius; y++)
    {
        for (int x = -radius; x <= radius; x++)
        {
            if (x*x + y*y <= radius*radius)
            {
                my_mlx_pixel_put(data, center_x + x, center_y + y, color);
            }
        }
    }
}
void clear_image(t_data *data, int color)
{
    int i, j;
    int width = data->width * 50;
    int height = data->height * 50;

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            my_mlx_pixel_put(data, j, i, color);
        }
    }
}
int render_frame(t_data *data)
{
    char prev_map[10][15];
    int block_size = 50;
    int color;

    for (int y = 0; y < data->height; y++)
    {
        for (int x = 0; x < data->width; x++)
        {
            // Only redraw if the map cell has changed
            if (data->map[y][x] != prev_map[y][x])
            {
                if (data->map[y][x] == '1')
                    color = 0x00FF0000; // Red for walls
                else
                    color = 0x00000000; // Black for empty space

                // Fill the block
                for (int i = 0; i < block_size; i++)
                {
                    for (int j = 0; j < block_size; j++)
                    {
                        my_mlx_pixel_put(data, x * block_size + j, y * block_size + i, color);
                    }
                }

                prev_map[y][x] = data->map[y][x];
            }

            // Draw border lines (always, to ensure they're visible)
            for (int i = 0; i < block_size; i++)
            {
                my_mlx_pixel_put(data, (x + 1) * block_size - 1, y * block_size + i, 0x00FFFFFF);
                my_mlx_pixel_put(data, x * block_size + i, (y + 1) * block_size - 1, 0x00FFFFFF);
            }
        }
    }

    // Draw player (assuming player coordinates are in map units, not pixels)
    int player_x = data->player.x * block_size + block_size / 2;
    int player_y = data->player.y * block_size + block_size / 2;
     draw_circle(data, player_x, player_y, data->player.radius,0x0000FF00);
   draw_line(data, player_x, player_y,40, 0x0000FF00);
    mlx_put_image_to_window(data->mlx, data->win, data->img, 0, 0);
    return (0);
}

int game_loop(t_data *data)
{
    mlx_clear_window(data->mlx, data->win);
    update_game(data);
    render_frame(data);
    return (0);
}
int	main(void)
{
	t_data data;
	char initial_map[10][15] = {"111111111111111", "100000000011001",
		"100000101111111", "111100000011111", "100000000000001",
		"100011000000001", "100000001010001", "101010000000011",
		"100000000000001", "111111111111111"};

	data.width = 15;
	data.height = 10;
	data.player.x = data.width / 2;
	data.player.y = data.height / 2;
	data.player.radius = 3;
	data.player.turn_dir = 0;
	data.player.walk_dir = 0;
    data.player.rotation_angle = PI / 2;
	data.player.moves_speed = 0.15;
	data.player.rotation_speed = 8 * (PI / 180);
    data.update_needed = 0;

	// Initialize MLX
	data.mlx = mlx_init();
	if (!data.mlx)
		return (1);

	// Create window
	data.win = mlx_new_window(data.mlx, data.width * 50, data.height * 50,
			"My Game");
	if (!data.win)
	{
		mlx_destroy_display(data.mlx);
		free(data.mlx);
		return (1);
	}

	// Allocate and initialize map
	if (!(data.map = malloc(data.height * sizeof(char *))))
	{
		mlx_destroy_window(data.mlx, data.win);
		mlx_destroy_display(data.mlx);
		free(data.mlx);
		return (1);
	}
	for (int i = 0; i < data.height; i++)
	{
		if (!(data.map[i] = malloc(data.width * sizeof(char))))
		{
			for (int j = 0; j < i; j++)
				free(data.map[j]);
			free(data.map);
			mlx_destroy_window(data.mlx, data.win);
			mlx_destroy_display(data.mlx);
			free(data.mlx);
			return (1);
		}
		memcpy(data.map[i], initial_map[i], data.width);
	}

	// Initialize image
	init_image(&data);

	// Set up hooks
	mlx_hook(data.win, 2, 1L << 0, key_press, &data);
   mlx_hook(data.win, 3, 1L << 1, key_release, &data);
	mlx_hook(data.win, 17, 1L << 17, close_window, &data);
    mlx_loop_hook(data.mlx, game_loop, &data);
	mlx_loop_hook(data.mlx, render_frame, &data);

	// Start the game loop
	mlx_loop(data.mlx);

	return (0);
}