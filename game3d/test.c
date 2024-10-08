#include "cub3D.h"
#define mini_map 0.2
void init_image(t_data *data)
{
    int bpp, size_line, endian;
    // Double the height for split screen
    data->img = mlx_new_image(data->mlx, data->width * data->player.block_size * 2, 
                              data->height * data->player.block_size * 2);
    data->addr = mlx_get_data_addr(data->img, &bpp, &size_line, &endian);
    data->bits_per_pixel = bpp;
    data->line_length = size_line;
    data->endian = endian;
}

void my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
    char *dst;

    dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
    *(unsigned int *)dst = color;
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
int is_position_invalid(t_data *data, float x, float y)
{
    // Convert coordinates to grid position (assuming grid size of 1)
    int grid_x = (int)(x + 0.5);
    int grid_y = (int)(y + 0.5);

    // Check if position is outside the map
    if (grid_x < 0 || grid_x >= data->width || grid_y < 0 || grid_y >= data->height)
        return 1; // Outside of the map

    // Check if position is a wall
   // printf("x = %d and y = %d and my char in the mao is %c\n", grid_x, grid_y, data->map[grid_y][grid_x]);
    if (data->map[grid_y][grid_x] == '1')
        return 1; // It's a wall

    return 0; // Position is valid (not a wall and within bounds)
}
void update_game(t_data *data)
{
    if (data->update_needed)
    {
        // Calculate potential new position
        float new_x = data->player.x + cos(data->player.rotation_angle) * data->player.moves_speed * data->player.walk_dir;
        float new_y = data->player.y + sin(data->player.rotation_angle) * data->player.moves_speed * data->player.walk_dir;

        // Check if the new position is not a wall (assuming you have a function to check for walls)
        // pprintf("my data is %2.f and y = %2.f\n", new_x, new_y);
        if (!is_position_invalid(data, new_x, new_y))
        {
            data->player.x = new_x;
            data->player.y = new_y;
        }

        // Update player rotation based on turn_dir
        data->player.rotation_angle += data->player.turn_dir * data->player.rotation_speed;

        // Normalize rotation angle to keep it within 0 to 2*PI
        if (data->player.rotation_angle >= 2 * PI)
        {
            data->player.rotation_angle -= 2 * PI;
        }
        else if (data->player.rotation_angle < 0)
        {
            data->player.rotation_angle += 2 * PI;
        }

        data->update_needed = 0;
    }
}
int key_press(int keycode, t_data *data)
{
    // printf("my data is %d\n", keycode);
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
void draw_line(t_data *data, int center_x, int center_y, int color, int flag)
{
    for (int i = -(FOV / 2) ;i <= (FOV / 2); i++)
    {
        // Calculate ray 
         double ray_angle = data->player.rotation_angle + i * (PI / 180);

        
        // Calculate ray direction
        double rayDirX = cos(ray_angle);
        double rayDirY = sin(ray_angle);

        // Get player's current grid position
        int mapX = center_x / data->player.block_size;
        int mapY = center_y / data->player.block_size;

        // Calculate delta distances
        double deltaDistX = fabs(1 / rayDirX);
        double deltaDistY = fabs(1 / rayDirY);

        // Calculate step and initial sideDistX/Y
        int stepX;
        int stepY;
        double sideDistX;
        double sideDistY;

        // Calculate X stepping and sideDist
        if (rayDirX < 0)
        {
            stepX = -1;
            sideDistX = (center_x - mapX * data->player.block_size) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = ((mapX + 1) * data->player.block_size - center_x) * deltaDistX;
        }

        // Calculate Y stepping and sideDist
        if (rayDirY < 0)
        {
            stepY = -1;
            sideDistY = (center_y - mapY * data->player.block_size) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = ((mapY + 1) * data->player.block_size - center_y) * deltaDistY;
        }

        // DDA algorithm
        int hit = 0;
        int side;
        double length = 0;

        while (hit == 0)
        {
            // Jump to next square
            if (sideDistX < sideDistY)
            {
                sideDistX += deltaDistX * data->player.block_size;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY * data->player.block_size;
                mapY += stepY;
                side = 1;
            }

            // Check if ray hit a wall
            if (mapX < 0 || mapY < 0 || mapX >= data->width || mapY >= data->height)
                break;
            if (data->map[mapY][mapX] == '1')
                hit = 1;
        }

        // Calculate distance to the wall
        if (side == 0)
            length = (mapX * data->player.block_size - center_x + (1 - stepX) * data->player.block_size / 2) / rayDirX;
        else
            length = (mapY * data->player.block_size - center_y + (1 - stepY) * data->player.block_size / 2) / rayDirY;

        // Draw the ray
        for (double j = 0; j <= length; j += 0.5)
        {
            double x = center_x + j * rayDirX;
            double y = center_y + j * rayDirY;
            if(i == 0)
                my_mlx_pixel_put(data, x, y, 0x0000FF);
            else
                my_mlx_pixel_put(data, x, y, color);
        }
    }
}
void draw_circle(t_data *data, int center_x, int center_y, int radius, int color)
{
    for (int y = -radius; y <= radius; y++)
    {
        for (int x = -radius; x <= radius; x++)
        {
            if (x * x + y * y <= radius * radius)
            {
                my_mlx_pixel_put(data, center_x + x, center_y + y, color);
            }
        }
    }
}
void draw_3d_walls(t_data *data)
{
    int screen_height = data->height * data->player.block_size;
    int screen_width = data->width * data->player.block_size;
    
    for (int x = 0; x < screen_width; x++)
    {
        // Calculate ray angle
        double ray_angle = data->player.rotation_angle - (FOV / 2) * (PI / 180) + 
                   (x * FOV * (PI / 180)) / screen_width;
        
        // Normalize angle
        if (ray_angle < 0) ray_angle += 2 * PI;
        if (ray_angle > 2 * PI) ray_angle -= 2 * PI;

        // Ray casting variables
        double rayDirX = cos(ray_angle);
        double rayDirY = sin(ray_angle);
        
        int mapX = (int)data->player.x;
        int mapY = (int)data->player.y;
        
        double deltaDistX = fabs(1 / rayDirX);
        double deltaDistY = fabs(1 / rayDirY);
        
        int stepX = (rayDirX < 0) ? -1 : 1;
        int stepY = (rayDirY < 0) ? -1 : 1;
        
        double sideDistX = (rayDirX < 0) ? 
            (data->player.x - mapX) * deltaDistX : 
            (mapX + 1.0 - data->player.x) * deltaDistX;
        double sideDistY = (rayDirY < 0) ? 
            (data->player.y - mapY) * deltaDistY : 
            (mapY + 1.0 - data->player.y) * deltaDistY;
        
        // DDA algorithm
        int hit = 0, side;
        while (hit == 0)
        {
            if (sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            
            if (mapX < 0 || mapY < 0 || mapX >= data->width || mapY >= data->height)
                break;
            if (data->map[mapY][mapX] == '1')
                hit = 1;
        }
        
        // Calculate wall height
        double perpWallDist;
        if (side == 0)
            perpWallDist = (mapX - data->player.x + (1 - stepX) / 2) / rayDirX;
        else
            perpWallDist = (mapY - data->player.y + (1 - stepY) / 2) / rayDirY;
        
        int lineHeight = (int)(screen_height / perpWallDist);
        
        // Calculate drawing bounds
        int drawStart = -lineHeight / 2 + screen_height / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + screen_height / 2;
        if (drawEnd >= screen_height) drawEnd = screen_height - 1;
        
        // Draw the walls
        int color = (side == 1) ? 0x00AA0000 : 0x00FF0000; // Darker color for y-side
        for (int y = drawStart; y < drawEnd; y++)
        {
            my_mlx_pixel_put(data, x, y, color);
        }
        
        // Draw ceiling
        for (int y = 0; y < drawStart; y++)
        {
            my_mlx_pixel_put(data, x, y, 0x00444444);
        }
        
        // Draw floor
        for (int y = drawEnd; y < screen_height; y++)
        {
            my_mlx_pixel_put(data, x, y, 0x00222222);
        }
    }
}
int render_frame(t_data *data)
{
    int color;
    
    // Clear the image first
    memset(data->addr, 0, data->line_length * data->height * data->player.block_size );

    // Draw 3D walls in the top half
    draw_3d_walls(data);

    // Draw 2D map in the bottom half
    int map_start_y = data->height * data->player.block_size ; // Start of 2D map
    for (int y = 0; y < data->height; y++)
    {
        for (int x = 0; x < data->width; x++)
        {
            if (data->map[y][x] == '1')
                color = 0x00FF0000; // Red for walls
            else
                color = 0x00000000; // Black for empty space

            // Fill the block
            for (int i = 0; i < data->player.block_size; i++)
            {
                for (int j = 0; j < data->player.block_size; j++)
                {
                    my_mlx_pixel_put(data, x * data->player.block_size + j, 
                                    map_start_y + y * data->player.block_size + i, color);
                }
            }
            
            // Draw grid lines
            for (int i = 0; i < data->player.block_size; i++)
            {
                my_mlx_pixel_put(data, (x + 1) * data->player.block_size - 1, 
                                map_start_y + y * data->player.block_size + i, 0x00FFFFFF);
                my_mlx_pixel_put(data, x * data->player.block_size + i, 
                                map_start_y + (y + 1) * data->player.block_size - 1, 0x00FFFFFF);
            }
        }
    }

    // Draw player on 2D map
    int player_x = data->player.x * data->player.block_size + data->player.block_size / 2;
    int player_y = map_start_y + data->player.y * data->player.block_size + data->player.block_size / 2;
    draw_circle(data, player_x, player_y, data->player.radius, 0x0000FF00);
    draw_line(data, player_x, player_y, 0x0000FF00, 0);

    // Put image to window
    mlx_put_image_to_window(data->mlx, data->win, data->img, 0, 0);
    return (0);
}


int game_loop(t_data *data)
{
    update_game(data);
    render_frame(data);
    return (0);
}
int main(void)
{
    t_data data;
    char initial_map[15][15] = {"111111111111111", "100000000011001",
                                "100000101111111", "111100000011111", "100000000000001",
                                "100011000000001", "100000001010001", "101010000000011",
                                "100000000000001","100000000000001","100000000000001","100000000000001","100000000000001","100000000000001","100000000000001","111111111111111"};

    data.width = 15;
    data.height = 10;
    data.player.block_size = 50 * mini_map;
    data.player.x = data.width / 2;
    data.player.y = data.height / 2;
    data.player.radius = 3 * mini_map;
    data.player.turn_dir = 0;
    data.player.walk_dir = 0;
    data.player.rotation_angle = PI / 2;
    data.player.moves_speed = 0.15;
    data.player.rotation_speed = 3 * (PI / 180);
    data.update_needed = 0;

    // Initialize MLX
    data.mlx = mlx_init();
    if (!data.mlx)
        return (1);

    // Create window
    data.win = mlx_new_window(data.mlx, data.width * 50, data.height * 50 * 2,
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