#ifndef CUB3D_H
#define CUB3D_H
#include <mlx.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#define PI 3.1415926535
#define FOV 60
#define NUM_RAYS 320

typedef struct s_player
{
    float x;
    float y;
    float dir;
} t_player;

typedef struct s_ray
{
    float angle;
    float distance;
} t_ray;

typedef struct s_data
{
    void *mlx;
    void *win;
    void *img;
    char *addr;
    int bits_per_pixel;
    int line_length;
    int endian;
    int width;
    int height;
    char **map;
    t_player player;
    t_ray rays[NUM_RAYS];
} t_data;

// Function prototypes
void init_image(t_data *data);
void my_mlx_pixel_put(t_data *data, int x, int y, int color);
int close_window(t_data *data);
int key_press(int keycode, t_data *data);
int render_frame(t_data *data);

void	ft_right(t_data *data);
void	ft_left(t_data *data);
void	ft_up(t_data *data);
void	ft_down(t_data *data);

#endif