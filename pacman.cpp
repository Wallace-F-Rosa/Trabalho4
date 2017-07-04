#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "allegro5/allegro_image.h"
#include <iostream>
#include <cstring>

using namespace std;

const float FPS = 5;
const int SCREEN_W = 500;
const int SCREEN_H = 550;

enum MYKEYS
{
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
};

//matriz definindo mapa do jogo: 1 representa paredes, 0 representa corredor
char MAPA[26][26] =
{
    "1111111111111111111111111",
    "1000000001111111000000001",
    "1011111101111111011111101",
    "1011111100000000011111101",
    "1000000001111111000000001",
    "1110111101111111011110111",
    "1000111100001000011110001",
    "1010111111101011111110101",
    "1010000111001001110000101",
    "1011110111011101110111101",
    "1011110100000000010111101",
    "1000110001111111000110001",
    "1110111101100011011110111",
    "1000000001100011000000001",
    "1011111101111111011111101",
    "1011100000000000000011101",
    "1000001111110111111000001",
    "1111101110000000111011111",
    "1111101110111110111011111",
    "1000000000000000000000001",
    "1011101111110111111011101",
    "1000001111110111111000001",
    "1011100000010100000011101",
    "1000001111000001111000001",
    "1111111111111111111111111",
};

struct GHOST{
    ALLEGRO_BITMAP *ghost   = NULL;
    int posx;
    int posy;
};

ALLEGRO_DISPLAY *display = NULL; // interface
ALLEGRO_AUDIO_STREAM *musica_tela_start = NULL; // música da tela de start
ALLEGRO_AUDIO_STREAM *musica_start_game = NULL; // musica do inicio do jogo
ALLEGRO_SAMPLE *sample_tela_start = NULL; //musica da tela de start
ALLEGRO_SAMPLE *sample_start_game = NULL;

ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_BITMAP *bouncer = NULL;
ALLEGRO_BITMAP *mapa   = NULL; //mapa do jogo
ALLEGRO_BITMAP *pacman   = NULL; //imagem do pacman
ALLEGRO_BITMAP *ready   = NULL; // imagem READY antes de começar o jogo
ALLEGRO_BITMAP *tela_start   = NULL; //tela de start

GHOST ghost[4];

int i = 15, j = 12; //posição inicial do Pacman na matriz
int q = 20; //tamanho de cada célula no mapa
int posy = i*q;
int posx = j*q;
int x_Ready= 9.4 * q, y_Ready=12 * q ; // posição da imagem READY!
char pac_estado;

bool key[4] = { false, false, false, false };
bool redraw = true;
bool sair = false;


void destroy_all()
{
    al_destroy_audio_stream(musica_start_game);
    al_destroy_audio_stream(musica_tela_start);
    al_destroy_sample(sample_start_game);
    al_destroy_sample(sample_tela_start);
    al_destroy_bitmap(bouncer);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
}


int inicializa() {
    //iniciando bibliotecas, addons e arquivos necessários para rodar o pacman
    if(!al_init())
    {
        cout << "Falha ao carregar Allegro" << endl;
        return 0;
    }

    if (!al_install_audio())
    {
        cout << "Falha ao inicializar audio"<<endl;
        return 0;
    }

    if (!al_init_acodec_addon())
    {
        cout << "Falha ao inicializar codecs de audio"<<endl;
        return 0;
    }

    if (!al_reserve_samples(1))
    {
        cout << "Falha ao alocar canais de audio"<<endl;
        return 0;
    }
    sample_start_game = al_load_sample( "sound/start.wav" );
    sample_tela_start = al_load_sample( "sound/start.wav" );

    if(!al_install_keyboard())
    {
        cout << "Falha ao inicializar o teclado" << endl;
        return 0;
    }

    timer = al_create_timer(1.1 / FPS);
    if(!timer)
    {
        cout << "Falha ao inicializar o temporizador" << endl;
        return 0;
    }

    if(!al_init_image_addon())
    {
        cout <<"Falha ao iniciar al_init_image_addon!" << endl;
        return 0;
    }

    display = al_create_display(SCREEN_W, SCREEN_H);
    if(!display)
    {
        cout << "Falha ao inicializar a tela" << endl;
        al_destroy_timer(timer);
        return 0;
    }

    mapa = al_load_bitmap("img/maps/map.bmp");
    if(!mapa)
    {
        cout << "Falha ao carregar o mapa!" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(mapa,0,0,0);

    tela_start = al_load_bitmap("img/maps/start1.bmp");
    if(!tela_start)
    {
        cout << "Falha ao carregar a tela de start!" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(tela_start,0,0,0);

    pacman = al_load_bitmap("img/pacman/pacman.tga");
    if(!pacman)
    {
        cout << "Falha ao carregar o pacman!" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(pacman,posx,posy,0);

    ready = al_load_bitmap("img/maps/ready.bmp");
    if(!ready)
    {
        cout << "Falha ao carregar a imagem READY!" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(ready,x_Ready,y_Ready,0);



    event_queue = al_create_event_queue();
    if(!event_queue)
    {
        cout << "Falha ao criar a fila de eventos" << endl;
        al_destroy_display(display);
        al_destroy_timer(timer);
        return 0;
    }

    musica_start_game = al_load_audio_stream("sound/start.wav", 5, 1024);

    if (!musica_start_game)
    {
        cout <<  "Falha ao carregar audio.\n"<<endl;
        al_destroy_event_queue(event_queue);
        al_destroy_display(display);
        al_destroy_timer(timer);
        al_destroy_sample(sample_start_game);
        return 0;
    }

    musica_tela_start = al_load_audio_stream("sound/start.wav", 5, 1024);

    if (!musica_tela_start)
    {
        cout <<  "Falha ao carregar audio.\n"<<endl;
        al_destroy_event_queue(event_queue);
        al_destroy_display(display);
        al_destroy_timer(timer);
        al_destroy_sample(sample_tela_start);
        return 0;
    }


    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    al_clear_to_color(al_map_rgb(0,0,0));
    al_flip_display();
    al_start_timer(timer);

    return 1;
}

void start()
{
    al_play_sample(sample_start_game, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    al_clear_to_color(al_map_rgb(0,0,0));
    al_draw_bitmap(mapa,0,0,0);
    al_draw_bitmap(pacman,posx,posy,0);
    al_draw_bitmap(ready,x_Ready,y_Ready,0);
    al_flip_display();
    al_rest(4.0);
    al_clear_to_color(al_map_rgb(0,0,0));
    al_draw_bitmap(mapa,0,0,0);
    al_draw_bitmap(pacman,posx,posy,0);
    al_flip_display();
}


void draw_tela_start()
{
    //Inicio do jogo
    //Enquanto a música de intro não acabar não inicia o jogo
    bool sair=false;
    bool animacao = true;
    while(!sair)
    {

        al_play_sample(sample_tela_start, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch(ev.keyboard.keycode)
            {
                case ALLEGRO_KEY_ENTER:
                    sair = true;
                    break;
            }
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            break;
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch(ev.keyboard.keycode)
            {
                case ALLEGRO_KEY_ENTER:
                    sair = true;
                    break;

                case ALLEGRO_KEY_ESCAPE:
                    destroy_all();
                    break;
            }
        }

        if(animacao)
            tela_start = al_load_bitmap("img/maps/start1.bmp");
        else
            tela_start = al_load_bitmap("img/maps/start2.bmp");
        al_draw_bitmap(tela_start,0,0,0);
        al_flip_display();
        al_rest(0.2);
        animacao = !animacao;

    }


    al_destroy_sample(sample_tela_start);
}


char * caminho(bool munch,char key)
{
    /*
        muda a imagem de acordo com a direção que o pacman está andando:
        para cima  'UP' = pacmanU
        para baixo 'DOWN' = pacmanD
        para direita 'RIGHT' = pacmanR
        para esquerda 'LEFT' = pacmanL
*/
    char * arquivo = new char[23];
    if(munch)
    {
        if(key == 'U')
            strcpy(arquivo,"img/pacman/pacman_munchU.tga");
        if(key == 'D')
            strcpy(arquivo,"img/pacman/pacman_munchD.tga");
        if(key == 'L')
            strcpy(arquivo,"img/pacman/pacman_munchL.tga");
        if(key == 'R')
            strcpy(arquivo,"img/pacman/pacman_munchR.tga");
    }
    else
    {
        if(key == 'U')
            strcpy(arquivo,"img/pacman/pacmanU.tga");
        if(key == 'D')
            strcpy(arquivo,"img/pacman/pacmanD.tga");
        if(key == 'L')
            strcpy(arquivo,"img/pacman/pacmanL.tga");
        if(key == 'R')
            strcpy(arquivo,"img/pacman/pacmanR.tga");
    }


    return arquivo;

}

void key_enable(int KEY,char celula)
{
    if(celula != '1')
    {
        if(KEY == KEY_UP)
        {
            pac_estado = KEY_UP;
            key[KEY_UP] = true;
            key[KEY_DOWN] = false;
            key[KEY_LEFT] = false;
            key[KEY_RIGHT] = false;
        }

        if(KEY == KEY_DOWN)
        {
            pac_estado = KEY_DOWN;
            key[KEY_DOWN] = true;
            key[KEY_UP] = false;
            key[KEY_LEFT] = false;
            key[KEY_RIGHT] = false;
        }

        if(KEY == KEY_LEFT)
        {
            pac_estado = KEY_LEFT;
            key[KEY_LEFT] = true;
            key[KEY_UP] = false;
            key[KEY_DOWN] = false;
            key[KEY_RIGHT] = false;
        }

        if(KEY == KEY_RIGHT)
        {
            pac_estado = KEY_RIGHT;
            key[KEY_RIGHT] = true;
            key[KEY_LEFT] = false;
            key[KEY_UP] = false;
            key[KEY_DOWN] = false;
        }
    }

}

void redraw_pacman()
{
    al_clear_to_color(al_map_rgb(0,0,0));
    al_draw_bitmap(mapa,0,0,0);
    al_draw_bitmap(pacman,posx,posy,0);
    al_flip_display();

}



void animacao(char estado)
{
    if(estado == 'U')
    {
        pacman = al_load_bitmap(caminho(false,'U'));
        redraw_pacman();
        al_rest(0.1);
        pacman = al_load_bitmap(caminho(true,'U'));
        redraw_pacman();
        al_rest(0.1);

    }
    if(estado == 'D')
    {
        pacman = al_load_bitmap(caminho(false,'D'));
        redraw_pacman();
        al_rest(0.1);
        pacman = al_load_bitmap(caminho(true,'D'));
        redraw_pacman();
        al_rest(0.1);
    }
    if(estado == 'L')
    {
        pacman = al_load_bitmap(caminho(false,'L'));
        redraw_pacman();
        al_rest(0.1);
        pacman = al_load_bitmap(caminho(true,'L'));
        redraw_pacman();
        al_rest(0.1);
    }
    if(estado== 'R')
    {
        pacman = al_load_bitmap(caminho(false,'R'));
        redraw_pacman();
        al_rest(0.1);
        pacman = al_load_bitmap(caminho(true,'R'));
        redraw_pacman();
        al_rest(0.1);
    }
}

int main(int argc, char **argv)
{
   // bool parado = true;


    if(!inicializa()) return -1;

    //tela de start
    draw_tela_start();

    //da inicio ao jogo
    start();

    while(!sair)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);



        if(ev.type == ALLEGRO_EVENT_TIMER)
        {
            if(key[KEY_UP] && MAPA[i-1][j] != '1')
            {
                pac_estado = 'U';// pacmanU = pacman Up
                i--;
                posy = i*q;



                //animacao(pac_estado,posx,posy,i,j);
            }

            else if(key[KEY_DOWN] && MAPA[i+1][j] != '1')
            {
                pac_estado = 'D';
                i++;
                posy = i*q;

            }

            else if(key[KEY_LEFT] && MAPA[i][j-1] != '1')
            {
                pac_estado = 'L';
                j--;
                posx = j*q;

            }

            else if(key[KEY_RIGHT] && MAPA[i][j+1] != '1')
            {
                pac_estado = 'R';
                j++;
                posx = j*q;
            }

            redraw = true;
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            break;
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch(ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                key_enable(KEY_UP,MAPA[i-1][j]);
                break;

            case ALLEGRO_KEY_DOWN:
                key_enable(KEY_DOWN,MAPA[i+1][j]);
                break;

            case ALLEGRO_KEY_LEFT:
                key_enable(KEY_LEFT,MAPA[i][j-1]);
                break;

            case ALLEGRO_KEY_RIGHT:
                key_enable(KEY_RIGHT,MAPA[i][j+1]);
                break;
            }
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch(ev.keyboard.keycode)
            {
                case ALLEGRO_KEY_UP:
                    key_enable(KEY_UP,MAPA[i-1][j]);
                    break;

                case ALLEGRO_KEY_DOWN:
                    key_enable(KEY_DOWN,MAPA[i+1][j]);
                    break;

                case ALLEGRO_KEY_LEFT:
                    key_enable(KEY_LEFT,MAPA[i][j-1]);
                    break;

                case ALLEGRO_KEY_RIGHT:
                    key_enable(KEY_RIGHT,MAPA[i][j+1]);
                    break;

                case ALLEGRO_KEY_ESCAPE:
                    sair = true;
                    break;
            }

        }

        if(redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;

            animacao(pac_estado);
        }
    }

    destroy_all();


    return 0;
}
