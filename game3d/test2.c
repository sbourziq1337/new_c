void ft_draw_wall3d(t_data *data)
{
    if (!data->length || data->height <= 0)
        return;

    int screen_height = data->height * data->player.block_size;
    int screen_width = data->width * data->player.block_size;

    for (int screen_x = 0; screen_x < screen_width; screen_x++)
    {
        int ray_index = screen_x * (FOV / screen_width); // Calculate correct ray index based on screen_x

        if (ray_index >= FOV)
            ray_index = FOV - 1;

        if (data->length[ray_index] <= 0.1)
            continue;

        // Calculate the distance to the projection plane
        float distance_to_plane = (screen_width / 2) / tan((FOV / 2) * (PI / 180));

        // Calculate wall height
        int wall_height = (int)(data->player.block_size / data->length[ray_index] * distance_to_plane);
        if (wall_height > screen_height)
            wall_height = screen_height;

        // Calculate drawing bounds
        int draw_start = (screen_height - wall_height) / 2;
        int draw_end = (screen_height + wall_height) / 2;

        // Clamp values
        if (draw_start < 0)
            draw_start = 0;
        if (draw_end >= screen_height)
            draw_end = screen_height - 1;

        // Draw ceiling
        for (int y = 0; y < draw_start; y++)
            my_mlx_pixel_put(data, screen_x, y, 0x444444);

        // Draw wall
        for (int y = draw_start; y < draw_end; y++)
            my_mlx_pixel_put(data, screen_x, y, 0xFF0000);

        // Draw floor
        for (int y = draw_end; y < screen_height; y++)
            my_mlx_pixel_put(data, screen_x, y, 0x222222);
    }
}

















 float size = data->player.block_size * mini_map;
     int screen_height = data->height * data->player.block_size;
    int screen_width = data->width * data->player.block_size;

    int ray_index = 0;
    for (double i = -(FOV / 2); i <= (FOV / 2); i += 0.08)
    {
        // Calculate ray
        double ray_angle = data->player.rotation_angle + i * (PI / 180);

        // Normalize angle
        if (ray_angle < 0)
            ray_angle += 2 * PI;
        if (ray_angle > 2 * PI)
            ray_angle -= 2 * PI;

        // Ray casting variables
        double rayDirX = cos(ray_angle);
        double rayDirY = sin(ray_angle);

        int mapX = (int)data->player.x;
        int mapY = (int)data->player.y;

        double deltaDistX = fabs(1 / rayDirX);
        double deltaDistY = fabs(1 / rayDirY);

        int stepX;
        int stepY;

        double sideDistX;
        double sideDistY;

        if (rayDirX < 0)
        {
            stepX = -1;
           sideDistX = (data->player.x - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX =(mapX + 1.0 - data->player.x) * deltaDistX;
        }

        // Calculate Y stepping and sideDist
        if (rayDirY < 0)
        {
            stepY = -1;
           sideDistY = (data->player.y - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
           sideDistY = (mapY + 1.0 - data->player.y) * deltaDistY;
        }

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
        // Draw the ray
        for (double j = 0; j <= perpWallDist; j += 0.5)
        {
            int x = (int)(center_x + j * rayDirX);
            int y = (int)(center_y + j * rayDirY);
            if (i == 0)
                my_mlx_pixel_put(data, x, y, 0x0000FF);
            else
                my_mlx_pixel_put(data, x, y, color);
        }
        ray_index++;
    }
}