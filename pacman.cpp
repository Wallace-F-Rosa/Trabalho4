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
    "1000110001110111000110001",
    "1110111101110111011110111",
    "1000000001110111000000001",
    "1011111101110111011111101",
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

ALLEGRO_DISPLAY *display = NULL; // interface
ALLEGRO_AUDIO_STREAM *musica = NULL;
ALLEGRO_SAMPLE *sample = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_BITMAP *bouncer = NULL;
ALLEGRO_BITMAP *mapa   = NULL;
ALLEGRO_BITMAP *pacman   = NULL;
int i = 15, j = 12; //posição inicial do Pacman na matriz
int q = 20; //tamanho de cada célula no mapa
int posy = i*q;
int posx = j*q;

bool key[4] = { false, false, false, false };
bool redraw = true;
bool sair = false;

char * caminho(bool animacao,char key)
{
    char * arquivo = new char[23];
    if(animacao)
        strcpy(arquivo,"img/pacman/pacman.tga");
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


int inicializa() {
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
    sample = al_load_sample( "sound/start.wav" );

    if(!al_install_keyboard())
    {
        cout << "Falha ao inicializar o teclado" << endl;
        return 0;
    }

    timer = al_create_timer(1.0 / FPS);
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

    pacman = al_load_bitmap("img/pacman/pacman.tga");
    if(!pacman)
    {
        cout << "Falha ao carregar o pacman!" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(pacman,posx,posy,0);



    event_queue = al_create_event_queue();
    if(!event_queue)
    {
        cout << "Falha ao criar a fila de eventos" << endl;
        al_destroy_display(display);
        al_destroy_timer(timer);
        return 0;
    }



    musica = al_load_audio_stream("sound/start.wav", 5, 1024);

    if (!musica)
    {
        cout <<  "Falha ao carregar audio.\n"<<endl;
        al_destroy_event_queue(event_queue);
        al_destroy_display(display);
        al_destroy_timer(timer);
        al_destroy_sample(sample);
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
    //Inicio do jogo
    //Enquanto a música de intro não acabar não inicia o jogo
    al_clear_to_color(al_map_rgb(0,0,0));
    al_draw_bitmap(mapa,0,0,0);
    al_draw_bitmap(pacman,posx,posy,0);
    al_flip_display();
    al_play_sample(sample, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

    al_rest(4.0);
}

int main(int argc, char **argv)
{
   // bool parado = true;
   bool animacao = false;

    if(!inicializa()) return -1;

    //da inicio ao jogo
    start();

    while(!sair)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);



        animacao = !animacao;

        if(ev.type == ALLEGRO_EVENT_TIMER)
        {

            // as the user press the keys, pacman must move in the same direction as the key that the user pressed
            // the image of pacman will change according to the key pressed
            if(key[KEY_UP] && MAPA[i-1][j] != '1')
            {
                pacman = al_load_bitmap("img/pacman/pacmanU.tga"); // pacmanl = pacman Up
                i--;
                posy = i*q;
            }

            if(key[KEY_DOWN] && MAPA[i+1][j] != '1')
            {
                pacman = al_load_bitmap("img/pacman/pacmanD.tga"); // pacmanl = pacman Down
                i++;
                posy = i*q;
            }

            if(key[KEY_LEFT] && MAPA[i][j-1] != '1')
            {
                pacman = al_load_bitmap("img/pacman/pacmanL.tga"); // pacmanl = pacman Left
                j--;
                posx = j*q;
            }

            if(key[KEY_RIGHT] && MAPA[i][j+1] != '1')
            {
                pacman = al_load_bitmap("img/pacman/pacmanR.tga"); // pacman Right
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
                key[KEY_UP] = true;
                break;

            case ALLEGRO_KEY_DOWN:
                key[KEY_DOWN] = true;
                break;

            case ALLEGRO_KEY_LEFT:
                key[KEY_LEFT] = true;
                break;

            case ALLEGRO_KEY_RIGHT:
                key[KEY_RIGHT] = true;
                break;
            }
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch(ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                key[KEY_UP] = false;
                break;

            case ALLEGRO_KEY_DOWN:
                key[KEY_DOWN] = false;
                break;

            case ALLEGRO_KEY_LEFT:
                key[KEY_LEFT] = false;
                break;

            case ALLEGRO_KEY_RIGHT:
                key[KEY_RIGHT] = false;
                break;

            case ALLEGRO_KEY_ESCAPE:
                sair = true;
                break;
            }
        }

        if(redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;

            al_clear_to_color(al_map_rgb(0,0,0));

            al_draw_bitmap(mapa,0,0,0);
            al_draw_bitmap(pacman,posx,posy,0);
            al_flip_display();
        }
    }

    al_destroy_audio_stream(musica);
    al_destroy_sample(sample);
    al_destroy_bitmap(bouncer);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);


    return 0;
}
