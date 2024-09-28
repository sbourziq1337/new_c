#include "cub3D.h"

void init_image(t_data *data)
{
    int bpp, size_line, endian;
    data->img = mlx_new_image(data->mlx, data->width * 50, data->height * 50);
    data->addr = mlx_get_data_addr(data->img, &bpp, &size_line, &endian);
    data->bits_per_pixel = bpp;
    data->line_length = size_line;
    data->endian = endian;
}

void my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
    char *dst;

     dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
    *(unsigned int*)dst = color;
}

int close_window(t_data *data)
{
    mlx_destroy_image(data->mlx, data->img);
    mlx_destroy_window(data->mlx, data->win);
    // Free map memory
    for (int i = 0; i < data->height; i++)
        free(data->map[i]);
    free(data->map);
    exit(0);
}

int key_press(int keycode, t_data *data)
{
    printf("my data is %d\n", keycode);
	if (keycode == 65307)
		close_window(data);
	else if (keycode == 100 || keycode == 65363)
	{
		ft_right(data);
	}
	else if (keycode == 97 || keycode == 65361)
	{
		ft_left(data);
	}
	else if (keycode == 115 || keycode == 65364)
		ft_down(data);
	else if (keycode == 119 || keycode == 65362)
		ft_up(data);
	return (keycode);
}

int render_frame(t_data *data)
{
    static char prev_map[10][15];
    int block_size = 50;
    int x, y;

    for (y = 0; y < data->height; y++)
    {
        for (x = 0; x < data->width; x++)
        {
            if (data->map[y][x] != prev_map[y][x])
            {
                int color;
                if (data->map[y][x] == '1')
                    color = 0x00FF0000; // Red for walls
                else if (data->map[y][x] == 'P')
                    color = 0x0000FF00; // Green for player
                else
                    color = 0x00000000; // Black for empty space

                for (int i = 0; i < block_size; i++)
                {
                    for (int j = 0; j < block_size; j++)
                    {
                        my_mlx_pixel_put(data, x * block_size + i, y * block_size + j, color);
                    }
                }
                prev_map[y][x] = data->map[y][x];
            }
        }
    }

    mlx_put_image_to_window(data->mlx, data->win, data->img, 0, 0);
    return (0);
}

int main(void)
{
    t_data data;
    char initial_map[10][15] = {"111111111111111", "100000000011001",
            "100000101111111", "111100000011111", "100000000000001",
            "100011000000001", "10000000P010001", "101010000000011",
            "100000000000001", "111111111111111"};

    data.width = 15;
    data.height = 10;
    
    
    // Initialize MLX
    data.mlx = mlx_init();
    if (!data.mlx)
        return (1);

    // Create window
    data.win = mlx_new_window(data.mlx, data.width * 50, data.height * 50, "My Game");
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
    mlx_hook(data.win, 17, 1L << 17, close_window, &data);
    mlx_loop_hook(data.mlx, render_frame, &data);

    // Start the game loop
    mlx_loop(data.mlx);

    return (0);
}