#include "cub3D.h"

void ft_right(t_data *data)
{
    int x, y;
    
    for (y = 0; y < data->height; y++)
    {
        for (x = data->width - 1; x >= 0; x--)
        {
            if (data->map[y][x] == 'P')
            {
                if (x < data->width - 1 && data->map[y][x + 1] == '0')
                {
                    data->map[y][x] = '0';
                    data->map[y][x + 1] = 'P';
                    return;
                }
            }
        }
    }
}

void ft_left(t_data *data)
{
    int x, y;
    
    for (y = 0; y < data->height; y++)
    {
        for (x = 0; x < data->width; x++)
        {
            if (data->map[y][x] == 'P')
            {
                if (x > 0 && data->map[y][x - 1] == '0')
                {
                    data->map[y][x] = '0';
                    data->map[y][x - 1] = 'P';
                    return;
                }
            }
        }
    }
}

void ft_up(t_data *data)
{
    int x, y;
    
    for (y = 0; y < data->height; y++)
    {
        for (x = 0; x < data->width; x++)
        {
            if (data->map[y][x] == 'P')
            {
                if (y > 0 && data->map[y - 1][x] == '0')
                {
                    data->map[y][x] = '0';
                    data->map[y - 1][x] = 'P';
                    return;
                }
            }
        }
    }
}

void ft_down(t_data *data)
{
    int x, y;
    
    for (y = data->height - 1; y >= 0; y--)
    {
        for (x = 0; x < data->width; x++)
        {
            if (data->map[y][x] == 'P')
            {
                if (y < data->height - 1 && data->map[y + 1][x] == '0')
                {
                    data->map[y][x] = '0';
                    data->map[y + 1][x] = 'P';
                    return;
                }
            }
        }
    }
}