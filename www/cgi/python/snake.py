import os
os.environ['PYGAME_HIDE_SUPPORT_PROMPT'] = "hide"
import pygame
import random
 
pygame.init()

white = (255, 255, 255)
black = (0, 0, 0)
red = (213, 50, 80)
 
window_width = 600
window_height = 400
 
dis = pygame.display.set_mode((window_width, window_height))
pygame.display.set_caption('Snake Game')
 
clock = pygame.time.Clock()
 
snake_tile = 10
snake_speed = 15
 
font_style = pygame.font.SysFont("bahnschrift", 25)
score_font = pygame.font.SysFont("comicsansms", 35)
 
 
def score(score):
    value = score_font.render("Score: " + str(score), True, white)
    dis.blit(value, [0, 0])
 
def snake(snake_tile, snake_list):
    for x in snake_list:
        pygame.draw.rect(dis, white, [x[0], x[1], snake_tile, snake_tile])
 
 
def message(msg, color):
    mesg = font_style.render(msg, True, color)
    dis.blit(mesg, [window_width / 4.5, window_height / 5])
 
 
def gameLoop():
    game_over = False
    game_close = False
 
    x1 = window_width / 2
    y1 = window_height / 2
 
    x1_change = 0
    y1_change = 0
 
    snake_List = []
    snake_length = 1
 
    food_x = round(random.randrange(0, window_width - snake_tile) / 10.0) * 10.0
    food_y = round(random.randrange(0, window_height - snake_tile) / 10.0) * 10.0
 
    while not game_over:
 
        while game_close == True:
            dis.fill(black)
            message("You Lost! Press Q to quit or R to restart", red)
            score(snake_length - 1)
            pygame.display.update()
 
            for event in pygame.event.get():
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_q:
                        game_over = True
                        game_close = False
                    if event.key == pygame.K_r:
                        gameLoop()
 
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                game_over = True
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_LEFT:
                    x1_change = -snake_tile
                    y1_change = 0
                elif event.key == pygame.K_RIGHT:
                    x1_change = snake_tile
                    y1_change = 0
                elif event.key == pygame.K_UP:
                    y1_change = -snake_tile
                    x1_change = 0
                elif event.key == pygame.K_DOWN:
                    y1_change = snake_tile
                    x1_change = 0
 
        if x1 >= window_width or x1 < 0 or y1 >= window_height or y1 < 0:
            game_close = True
        x1 += x1_change
        y1 += y1_change
        dis.fill(black)
        pygame.draw.rect(dis, red, [food_x, food_y, snake_tile, snake_tile])
        snake_Head = []
        snake_Head.append(x1)
        snake_Head.append(y1)
        snake_List.append(snake_Head)
        if len(snake_List) > snake_length:
            del snake_List[0]
 
        for x in snake_List[:-1]:
            if x == snake_Head:
                game_close = True
 
        snake(snake_tile, snake_List)
        score(snake_length - 1)
 
        pygame.display.update()
 
        if x1 == food_x and y1 == food_y:
            food_x = round(random.randrange(0, window_width - snake_tile) / 10.0) * 10.0
            food_y = round(random.randrange(0, window_height - snake_tile) / 10.0) * 10.0
            snake_length += 1
 
        clock.tick(snake_speed)
 
    pygame.quit()
    print("Thanks for playing!")
    quit()

gameLoop()