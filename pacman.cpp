#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "allegro5/allegro_image.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

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
    "1000000001111111000000001", // 16
    "1011111101111111011111101", // 4
    "1011111100000000011111101", // 11
    "1000000001111111000000001", // 16
    "1110111101111111011110111", // 4
    "1000111100001000011110001", // 14
    "1010111111101011111110101", // 6
    "1010000111001001110000101", // 14
    "1011110111011101110111101", // 6
    "1011110100000000010111101", // 13
    "1000110001115111000110001", //12
    "1110111101155511011110111", // 4
    "0000000001155511000000000", // 18
    "1110111101111111011110111", // 4
    "1010100000007000000010101", // 16
    "1000001111110111111000001", // 11
    "1111101110000000111011111", // 9
    "1111101110111110111011111", // 4
    "1000000000000000000000001", // 23
    "1011101111110111111011101", // 5
    "1000001111110111111000001", // 11
    "1011100000010100000011101", // 15
    "1000001111000001111000001", // 15
    "1111111111111111111111111",
};

struct GHOST{
    ALLEGRO_BITMAP *ghost   = NULL;
    int posx;
    int posy;
};

ALLEGRO_DISPLAY *display = NULL; // interface
ALLEGRO_SAMPLE *sample_tela_start = NULL; //musica da tela de start
ALLEGRO_SAMPLE *sample_start_game = NULL; // musica do inicío do jogo
ALLEGRO_SAMPLE *sample_victory = NULL; // musica de vitória
ALLEGRO_SAMPLE *sample_munch = NULL; // som do pacman comendo a pilula
ALLEGRO_SAMPLE *sample_ghosts = NULL; // som do pacman comendo a pilula
ALLEGRO_SAMPLE *sample_game_over = NULL; // som do pacman comendo a pilula

ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_BITMAP *bouncer = NULL;
ALLEGRO_BITMAP *mapa   = NULL; //mapa do jogo
ALLEGRO_BITMAP *pacman   = NULL; //imagem do pacman
ALLEGRO_BITMAP *GBlue   = NULL;
ALLEGRO_BITMAP *GOrng   = NULL;
ALLEGRO_BITMAP *GPink   = NULL;
ALLEGRO_BITMAP *GRed   = NULL;
ALLEGRO_BITMAP *ready   = NULL; // imagem READY antes de começar o jogo
ALLEGRO_BITMAP *tela_start   = NULL; //tela de start
ALLEGRO_BITMAP *tela_game_over   = NULL; //desenha a imagem game_over

ALLEGRO_BITMAP *pillsr = NULL;
ALLEGRO_BITMAP *score = NULL;
ALLEGRO_BITMAP *zero = NULL;
ALLEGRO_BITMAP *um = NULL;
ALLEGRO_BITMAP *dois = NULL;
ALLEGRO_BITMAP *tres = NULL;
ALLEGRO_BITMAP *quatro = NULL;
ALLEGRO_BITMAP *cinco = NULL;
ALLEGRO_BITMAP *seis = NULL;
ALLEGRO_BITMAP *sete = NULL;
ALLEGRO_BITMAP *oito = NULL;
ALLEGRO_BITMAP *nove = NULL;




GHOST ghost[4];

int i = 15, j = 12; //posição inicial do Pacman na matriz
int q = 20; //tamanho de cada célula no mapa
int posy = i*q;
int posx = j*q;
int x_Ready= 9.4 * q, y_Ready=12 * q ; // posição da imagem READY!
int x_game_over= 6.4 * q, y_game_over=12 * q ; // posição da imagem READY!
char pac_estado;
float xscore = 1*q, yscore = 25.5*q;

int GBi = 12, GBj = 12;
int GBposy = GBi*q;
int GBposx = GBj*q;
char GBdir;

int GOi = 12, GOj = 12;
int GOposy = GOi*q;
int GOposx = GOj*q;
char GOdir;

int GPi = 12, GPj = 12;
int GPposy = GPi*q;
int GPposx = GPj*q;
char GPdir;

int GRi = 12, GRj = 12;
int GRposy = GRi*q;
int GRposx = GRj*q;
char GRdir;

float mpillsdy[25][25] = {0};
float mpillsdx[25][25] = {0};
int qscore = 0;
float xp1score = 6.1*q, xp2score = 7.42*q, xp3score = 8.7*q,xp4score = 10.1*q;
float yp1score = 25.5*q, yp2score = 25.5*q, yp3score = 25.5*q, yp4score = 25.5*q;

bool key[4] = { false, false, false, false };
bool redraw = true;
bool sair = false;


void destroy_all()
{

    al_destroy_sample(sample_game_over);
    al_destroy_sample(sample_ghosts);

    if(sample_munch)
    {
        al_destroy_sample(sample_munch);
        al_destroy_sample(sample_victory);
    }

    al_destroy_sample(sample_start_game);
    al_destroy_sample(sample_tela_start);
    al_destroy_bitmap(bouncer);
    al_destroy_bitmap(pacman);
    al_destroy_bitmap(mapa);
    al_destroy_bitmap(GBlue);
    al_destroy_bitmap(GOrng);
    al_destroy_bitmap(GRed);
    al_destroy_bitmap(GPink);
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
    sample_munch = al_load_sample( "sound/chomp.wav" );
    sample_victory = al_load_sample( "sound/intermission.wav" );
    sample_ghosts = al_load_sample( "sound/ghost.wav" );
    sample_game_over = al_load_sample( "sound/death.wav" );


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

    tela_game_over = al_load_bitmap("img/maps/game_over.bmp");
    if(!tela_start)
    {
        cout << "Falha ao carregar a tela de game over!" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(tela_game_over,x_game_over,y_game_over,0);

    pacman = al_load_bitmap("img/pacman/pacman.tga");
    if(!pacman)
    {
        cout << "Falha ao carregar o pacman!" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(pacman,posx,posy,0);

    GBlue = al_load_bitmap("img/ghosts/blue/blueU.tga");
    if(!GBlue)
    {
        cout << "Falha ao carregar o GBlue!" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(GBlue,GBposx,GBposy,0);

    GOrng = al_load_bitmap("img/ghosts/orange/orangeU.tga");
    if(!GOrng)
    {
        cout << "Falha ao carregar o GOrng!" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(GOrng,GOposx,GOposy,0);

    GPink = al_load_bitmap("img/ghosts/pink/pinkU.tga");
    if(!GPink)
    {
        cout << "Falha ao carregar o GPink!" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(GPink,GPposx,GPposy,0);

    GRed = al_load_bitmap("img/ghosts/red/redU.tga");
    if(!GRed)
    {
        cout << "Falha ao carregar o GRed!" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(GRed,GRposx,GRposy,0);

    ready = al_load_bitmap("img/maps/ready.bmp");
    if(!ready)
    {
        cout << "Falha ao carregar a imagem READY!" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(ready,x_Ready,y_Ready,0);

    pillsr = al_load_bitmap("img/maps/pillsr.bmp");
    if(!pillsr)
    {
        cout << "Falla ao carregar pills" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(pillsr,0,0,0);

    score = al_load_bitmap("img/maps/Score.bmp");
    if(!score)
    {
        cout << "Falha ao carregar score" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(score,xscore,yscore,0);

    zero = al_load_bitmap("img/maps/0.bmp");
    if(!zero)
    {
        cout << "Falha ao carregar 0" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(zero,0,0,0);

    um = al_load_bitmap("img/maps/1.bmp");
    if(!um)
    {
        cout << "Falha ao carregar um" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(um,0,0,0);

    dois = al_load_bitmap("img/maps/2.bmp");
    if(!dois)
    {
        cout << "Falha ao carregar dois" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(dois,0,0,0);

    tres = al_load_bitmap("img/maps/3.bmp");
    if(!tres)
    {
        cout << "Falha ao carregar tres" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(tres,0,0,0);

    quatro = al_load_bitmap("img/maps/4.bmp");
    if(!quatro)
    {
        cout << "Falha ao carregar quatro" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(quatro,0,0,0);

    cinco = al_load_bitmap("img/maps/5.bmp");
    if(!cinco)
    {
        cout << "Falha ao carregar cinco" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(cinco,0,0,0);

    seis = al_load_bitmap("img/maps/6.bmp");
    if(!seis)
    {
        cout << "Falha ao carregar seis" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(seis,0,0,0);

    sete = al_load_bitmap("img/maps/7.bmp");
    if(!sete)
    {
        cout << "Falha ao carregar sete" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(sete,0,0,0);

    oito = al_load_bitmap("img/maps/8.bmp");
    if(!oito)
    {
        cout << "Falha ao carregar oito" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(oito,0,0,0);

    nove = al_load_bitmap("img/maps/9.bmp");
    if(!nove)
    {
        cout << "Falha ao carregar nove" << endl;
        al_destroy_display(display);
        return 0;
    }
    al_draw_bitmap(nove,0,0,0);


    event_queue = al_create_event_queue();
    if(!event_queue)
    {
        cout << "Falha ao criar a fila de eventos" << endl;
        al_destroy_display(display);
        al_destroy_timer(timer);
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
    al_draw_bitmap(GBlue,GBposx,GBposy,0);
    al_draw_bitmap(GOrng,GOposx,GOposy,0);
    al_draw_bitmap(GPink,GPposx,GPposy,0);
    al_draw_bitmap(GRed,GRposx,GRposy,0);
    al_draw_bitmap(ready,x_Ready,y_Ready,0);
    al_flip_display();
    al_rest(4.0);
    al_clear_to_color(al_map_rgb(0,0,0));
    al_draw_bitmap(mapa,0,0,0);
    for (int i = 0;i<26;i++)
        for (int j = 0;j<26;j++)
            if (MAPA[i][j]=='0'){
                mpillsdx[i][j] = j*q;
                mpillsdy[i][j] = i*q;
                al_draw_bitmap(pillsr,mpillsdx[i][j],mpillsdy[i][j],0);
            }

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
                    destroy_all();
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

char * caminhoGB(char GBdir)
{
    char * arquivo = new char[23];
    {
        if(GBdir == 'U')
            strcpy(arquivo,"img/ghosts/blue/blueU.tga");
        if(GBdir == 'D')
            strcpy(arquivo,"img/ghosts/blue/blueD.tga");
        if(GBdir == 'L')
            strcpy(arquivo,"img/ghosts/blue/blueL.tga");
        if(GBdir == 'R')
            strcpy(arquivo,"img/ghosts/blue/blueR.tga");
    }

    return arquivo;

}

char * caminhoGO(char GOdir)
{
    char * arquivo = new char[23];
    {
        if(GOdir == 'U')
            strcpy(arquivo,"img/ghosts/orange/orangeU.tga");
        if(GOdir == 'D')
            strcpy(arquivo,"img/ghosts/orange/orangeD.tga");
        if(GOdir == 'L')
            strcpy(arquivo,"img/ghosts/orange/orangeL.tga");
        if(GOdir == 'R')
            strcpy(arquivo,"img/ghosts/orange/orangeR.tga");
    }

    return arquivo;

}

char * caminhoGP(char GPdir)
{
    char * arquivo = new char[23];
    {
        if(GPdir == 'U')
            strcpy(arquivo,"img/ghosts/pink/pinkU.tga");
        if(GPdir == 'D')
            strcpy(arquivo,"img/ghosts/pink/pinkD.tga");
        if(GPdir == 'L')
            strcpy(arquivo,"img/ghosts/pink/pinkL.tga");
        if(GPdir == 'R')
            strcpy(arquivo,"img/ghosts/pink/pinkR.tga");
    }

    return arquivo;

}

char * caminhoGR(char GRdir)
{
    char * arquivo = new char[23];
    {
        if(GRdir == 'U')
            strcpy(arquivo,"img/ghosts/red/redU.tga");
        if(GRdir == 'D')
            strcpy(arquivo,"img/ghosts/red/redD.tga");
        if(GRdir == 'L')
            strcpy(arquivo,"img/ghosts/red/redL.tga");
        if(GRdir == 'R')
            strcpy(arquivo,"img/ghosts/red/redR.tga");
    }

    return arquivo;

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

char GBmove(int GBposx, int GBposy)
{
    int counter = 0;
    bool valid = true;
    char opt[4] = {'D', 'U', 'R', 'L'};
    int rd;

    if(MAPA[GBposx+1][GBposy] != '1')
        counter++;
    if(MAPA[GBposx-1][GBposy] != '1')
        counter++;
    if(MAPA[GBposx][GBposy+1] != '1')
        counter++;
    if(MAPA[GBposx][GBposy-1] != '1')
        counter++;

    if(MAPA[GBposx][GBposy] == '5')
        return 'U';

    if(counter > 1)
    {
        while(valid)
        {
            rd = rand()%4;
            if(rd == 0)
                if(MAPA[GBposx+1][GBposy] != '1')
                    return 'D';
            if(rd == 1)
                if(MAPA[GBposx-1][GBposy] != '1')
                    return 'U';
            if(rd == 2)
                if(MAPA[GBposx][GBposy+1] != '1')
                    return 'R';
            if(rd == 3)
                if(MAPA[GBposx][GBposy-1] != '1')
                    return 'L';
        }
    }

}

char GOmove(int GOposx, int GOposy)
{
    int counter = 0;
    bool valid = true;
    char opt[4] = {'D', 'U', 'R', 'L'};
    int rd;

    if(MAPA[GOposx+1][GOposy] != '1')
        counter++;
    if(MAPA[GOposx-1][GOposy] != '1')
        counter++;
    if(MAPA[GOposx][GOposy+1] != '1')
        counter++;
    if(MAPA[GOposx][GOposy-1] != '1')
        counter++;

    if(MAPA[GOposx][GOposy] == '5')
        return 'U';

    if(counter > 1)
    {
        while(valid)
        {
            rd = rand()%4;
            if(rd == 0)
                if(MAPA[GOposx+1][GOposy] != '1')
                    return 'D';
            if(rd == 1)
                if(MAPA[GOposx-1][GOposy] != '1')
                    return 'U';
            if(rd == 2)
                if(MAPA[GOposx][GOposy+1] != '1')
                    return 'R';
            if(rd == 3)
                if(MAPA[GOposx][GOposy-1] != '1')
                    return 'L';
        }
    }

}

char GPmove(int GPposx, int GPposy)
{
    int counter = 0;
    bool valid = true;
    char opt[4] = {'D', 'U', 'R', 'L'};
    int rd;

    if(MAPA[GPposx+1][GPposy] != '1')
        counter++;
    if(MAPA[GPposx-1][GPposy] != '1')
        counter++;
    if(MAPA[GPposx][GPposy+1] != '1')
        counter++;
    if(MAPA[GPposx][GPposy-1] != '1')
        counter++;

    if(MAPA[GPposx][GPposy] == '5')
        return 'U';

    if(counter > 1)
    {
        while(valid)
        {
            rd = rand()%4;
            if(rd == 0)
                if(MAPA[GPposx+1][GPposy] != '1')
                    return 'D';
            if(rd == 1)
                if(MAPA[GPposx-1][GPposy] != '1')
                    return 'U';
            if(rd == 2)
                if(MAPA[GPposx][GPposy+1] != '1')
                    return 'R';
            if(rd == 3)
                if(MAPA[GPposx][GPposy-1] != '1')
                    return 'L';
        }
    }

}

char GRmove(int GRposx, int GRposy)
{
    int counter = 0;
    bool valid = true;
    char opt[4] = {'D', 'U', 'R', 'L'};
    int rd;

    if(MAPA[GRposx+1][GRposy] != '1')
        counter++;
    if(MAPA[GRposx-1][GRposy] != '1')
        counter++;
    if(MAPA[GRposx][GRposy+1] != '1')
        counter++;
    if(MAPA[GRposx][GRposy-1] != '1')
        counter++;

    if(MAPA[GRposx][GRposy] == '5')
        return 'U';

    if(counter > 1)
    {
        while(valid)
        {
            rd = rand()%4;
            if(rd == 0)
                if(MAPA[GRposx+1][GRposy] != '1')
                    return 'D';
            if(rd == 1)
                if(MAPA[GRposx-1][GRposy] != '1')
                    return 'U';
            if(rd == 2)
                if(MAPA[GRposx][GRposy+1] != '1')
                    return 'R';
            if(rd == 3)
                if(MAPA[GRposx][GRposy-1] != '1')
                    return 'L';
        }
    }

}

void victory()
{
    bool animacao = true;

    pacman = al_load_bitmap("img/pacman/pacman.tga");

    al_rest(1);
    for(int w = 0; w < 50; w++ )
    {
        al_play_sample(sample_victory, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

        if(animacao)
            mapa = al_load_bitmap( "img/maps/map.bmp" );
        else
            mapa = al_load_bitmap( "img/maps/map_victory.bmp" );


        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(mapa,0,0,0);
        al_draw_bitmap(pacman,posx,posy,0);
        al_flip_display();
        al_rest(0.08);
        animacao = !animacao;
    }

    destroy_all();


}

void end_game()
{
    al_destroy_sample(sample_game_over);
    al_destroy_sample(sample_start_game);
    al_destroy_sample(sample_tela_start);
    al_destroy_bitmap(bouncer);
    al_destroy_bitmap(pacman);
    al_destroy_bitmap(mapa);
    al_destroy_bitmap(GBlue);
    al_destroy_bitmap(GOrng);
    al_destroy_bitmap(GRed);
    al_destroy_bitmap(GPink);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);


}

void redraw_game_over()
{
    al_clear_to_color(al_map_rgb(0,0,0));
    al_draw_bitmap(mapa,0,0,0);
    al_draw_bitmap(tela_game_over,x_game_over,y_game_over,0);
}

void game_over()
{
    //interrpmpe os audios que estão sendo tocados
    al_destroy_sample(sample_ghosts);
    al_destroy_sample(sample_munch);


    //desenha animação de morte do pacman
    redraw_game_over();
    pacman = al_load_bitmap("img/pacman/death/pacdeadst1.tga");
    al_draw_bitmap(pacman,posx,posy,0);
    al_flip_display();

    al_rest(1);

    //poe a musica da animação de morte para tocar
    al_play_sample(sample_game_over, 1.0, 0.0, 1.05, ALLEGRO_PLAYMODE_ONCE, NULL);



    redraw_game_over();
    pacman = al_load_bitmap("img/pacman/death/pacdeadst2.tga");
    al_draw_bitmap(pacman,posx,posy,0);
    al_flip_display();
    al_rest(0.1);

    redraw_game_over();
    pacman = al_load_bitmap("img/pacman/death/pacdeadst3.tga");
    al_draw_bitmap(pacman,posx,posy,0);
    al_flip_display();
    al_rest(0.1);

    redraw_game_over();
    pacman = al_load_bitmap("img/pacman/death/pacdeadst4.tga");
    al_draw_bitmap(pacman,posx,posy,0);
    al_flip_display();
    al_rest(0.1);

    redraw_game_over();
    pacman = al_load_bitmap("img/pacman/death/pacdeadst5.tga");
    al_draw_bitmap(pacman,posx,posy,0);
    al_flip_display();
    al_rest(0.1);

    redraw_game_over();
    pacman = al_load_bitmap("img/pacman/death/pacdeadst6.tga");
    al_draw_bitmap(pacman,posx,posy,0);
    al_flip_display();
    al_rest(0.1);

    redraw_game_over();
    pacman = al_load_bitmap("img/pacman/death/pacdeadst7.tga");
    al_draw_bitmap(pacman,posx,posy,0);
    al_flip_display();
    al_rest(0.1);

    redraw_game_over();
    pacman = al_load_bitmap("img/pacman/death/pacdeadst8.tga");
    al_draw_bitmap(pacman,posx,posy,0);
    al_flip_display();
    al_rest(0.1);

    redraw_game_over();
    pacman = al_load_bitmap("img/pacman/death/pacdeadst9.tga");
    al_draw_bitmap(pacman,posx,posy,0);
    al_flip_display();
    al_rest(0.1);

    redraw_game_over();
    pacman = al_load_bitmap("img/pacman/death/pacdeadst10.tga");
    al_draw_bitmap(pacman,posx,posy,0);
    al_flip_display();
    al_rest(0.1);

    redraw_game_over();
    pacman = al_load_bitmap("img/pacman/death/pacdeadst11.tga");
    al_draw_bitmap(pacman,posx,posy,0);
    al_flip_display();
    al_rest(0.2);

    redraw_game_over();
    pacman = al_load_bitmap("img/pacman/death/pacdeadst12.tga");
    al_draw_bitmap(pacman,posx,posy,0);
    al_flip_display();
    al_rest(0.1);

    redraw_game_over();
    al_flip_display();
    al_rest(4);
    end_game();
}

void redraw_placar()
{

        al_draw_bitmap(pacman,posx,posy,0);
        al_draw_bitmap(GBlue,GBposx,GBposy,0);
        al_draw_bitmap(GOrng,GOposx,GOposy,0);
        al_draw_bitmap(GPink,GPposx,GPposy,0);
        al_draw_bitmap(GRed,GRposx,GRposy,0);
        al_draw_bitmap(score,xscore,yscore,0);
        int uni, dzn, cent, aux;
        uni = qscore%10;
        aux = qscore%100;
        dzn = aux/10;
        cent = qscore/100;

        //desenha un zero atrás do número para aumentar a sensação de recompensa
        al_draw_bitmap(zero,xp4score,yp4score,0);

        //contrela dos algarismos do placar
        if (uni==0){
            al_draw_bitmap(zero,xp3score,yp3score,0);
        }
        else if(uni==1){
            al_draw_bitmap(um,xp3score,yp3score,0);
        }
        else if(uni==2){
            al_draw_bitmap(dois,xp3score,yp3score,0);
        }
        else if(uni==3){
            al_draw_bitmap(tres,xp3score,yp3score,0);
        }
        else if(uni==4){
            al_draw_bitmap(quatro,xp3score,yp3score,0);
        }
        else if(uni==5){
            al_draw_bitmap(cinco,xp3score,yp3score,0);
        }
        else if(uni==6){
            al_draw_bitmap(seis,xp3score,yp3score,0);
        }
        else if(uni==7){
            al_draw_bitmap(sete,xp3score,yp3score,0);
        }
        else if(uni==8){
            al_draw_bitmap(oito,xp3score,yp3score,0);
        }
        else if (uni==9){
            al_draw_bitmap(nove,xp3score,yp3score,0);
        }

        if(qscore>9){
        if(dzn==0){
            al_draw_bitmap(zero,xp2score,yp2score,0);
        }
        else if(dzn==1){
            al_draw_bitmap(um,xp2score,yp2score,0);
        }
        else if(dzn==2){
            al_draw_bitmap(dois,xp2score,yp2score,0);
        }
        else if(dzn==3){
            al_draw_bitmap(tres,xp2score,yp2score,0);
        }
        else if(dzn==4){
            al_draw_bitmap(quatro,xp2score,yp2score,0);
        }
        else if(dzn==5){
            al_draw_bitmap(cinco,xp2score,yp2score,0);
        }
        else if(dzn==6){
            al_draw_bitmap(seis,xp2score,yp2score,0);
        }
        else if(dzn==7){
            al_draw_bitmap(sete,xp2score,yp2score,0);
        }
        else if(dzn==8){
            al_draw_bitmap(oito,xp2score,yp2score,0);
        }
        else if (dzn==9){
            al_draw_bitmap(nove,xp2score,yp2score,0);
        }
        }

        if(cent==1){
            al_draw_bitmap(um,xp1score,yp1score,0);
        }
        else if(cent==2){
            al_draw_bitmap(dois,xp1score,yp1score,0);
        }
        else if(cent==3){
            al_draw_bitmap(tres,xp1score,yp1score,0);
        }
        else if(cent==4){
            al_draw_bitmap(quatro,xp1score,yp1score,0);
        }
        else if(cent==5){
            al_draw_bitmap(cinco,xp1score,yp1score,0);
        }
        else if(cent==6){
            al_draw_bitmap(seis,xp1score,yp1score,0);
        }
        else if(cent==7){
            al_draw_bitmap(sete,xp1score,yp1score,0);
        }
        else if(cent==8){
            al_draw_bitmap(oito,xp1score,yp1score,0);
        }
        else if (cent==9){
            al_draw_bitmap(nove,xp1score,yp1score,0);
        }

        al_flip_display();
}



void redraw_map(){

    if(qscore >= 252)
    {
        redraw_placar();
        victory();
    }
    else
    {
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(mapa,0,0,0);
        for (int i = 0;i<26;i++)
            for (int j = 0;j<26;j++)
                if (MAPA[i][j]=='0')
                {
                    if(posx == mpillsdx[i][j] && posy == mpillsdy[i][j])
                    {
                        al_play_sample(sample_munch, 1.0, 0.0, 1.05, ALLEGRO_PLAYMODE_ONCE, NULL);
                        //al_draw_bitmap(pillsr,mpillsdx[i][j],mpillsdy[i][j],0);
                        mpillsdx[i][j] = 1200 * q;
                        mpillsdy[i][j] = 1200 * q;

                    }
                    else if(mpillsdx[i][j] != 0 || mpillsdy[i][j] != 0)
                        al_draw_bitmap(pillsr,mpillsdx[i][j],mpillsdy[i][j],0);

                }

        redraw_placar();

    }
}

void redraw_pacman()
{
    al_clear_to_color(al_map_rgb(0,0,0));
    al_draw_bitmap(mapa,0,0,0);

    if(pac_estado == 'R')
    {

        if(j == 25  && i == 13)
        {
            j = 0;
            posx = j*q;
            posy = 13*q;
        }
    }

    if(pac_estado == 'L')
    {
        if(j == -1 && i == 13)
        {
            j = 25;
            posx = j * q;
            posy = 13*q;
        }

    }


    al_draw_bitmap(pacman,posx,posy,0);
    al_draw_bitmap(GBlue,GBposx,GBposy,0);
    al_draw_bitmap(GOrng,GOposx,GOposy,0);
    al_draw_bitmap(GPink,GPposx,GPposy,0);
    al_draw_bitmap(GRed,GRposx,GRposy,0);

}

void animacao(char estado)
{
    if(estado == 'U')
    {
        pacman = al_load_bitmap(caminho(false,'U'));
        GBlue = al_load_bitmap(caminhoGB(GBdir));
        GOrng = al_load_bitmap(caminhoGO(GOdir));
        GPink = al_load_bitmap(caminhoGP(GPdir));
        GRed = al_load_bitmap(caminhoGR(GRdir));
        redraw_pacman();
        redraw_map();
        al_rest(0.1);
        pacman = al_load_bitmap(caminho(true,'U'));
        GBlue = al_load_bitmap(caminhoGB(GBdir));
        GOrng = al_load_bitmap(caminhoGO(GOdir));
        GPink = al_load_bitmap(caminhoGP(GPdir));
        GRed = al_load_bitmap(caminhoGR(GRdir));
        redraw_pacman();
        redraw_map();
        al_rest(0.1);

    }
    if(estado == 'D')
    {
        pacman = al_load_bitmap(caminho(false,'D'));
        GBlue = al_load_bitmap(caminhoGB(GBdir));
        GOrng = al_load_bitmap(caminhoGO(GOdir));
        GPink = al_load_bitmap(caminhoGP(GPdir));
        GRed = al_load_bitmap(caminhoGR(GRdir));
        redraw_pacman();
        redraw_map();
        al_rest(0.1);
        pacman = al_load_bitmap(caminho(true,'D'));
        GBlue = al_load_bitmap(caminhoGB(GBdir));
        GOrng = al_load_bitmap(caminhoGO(GOdir));
        GPink = al_load_bitmap(caminhoGP(GPdir));
        GRed = al_load_bitmap(caminhoGR(GRdir));
        redraw_pacman();
        redraw_map();
        al_rest(0.1);
    }
    if(estado == 'L')
    {
        pacman = al_load_bitmap(caminho(false,'L'));
        GBlue = al_load_bitmap(caminhoGB(GBdir));
        GOrng = al_load_bitmap(caminhoGO(GOdir));
        GPink = al_load_bitmap(caminhoGP(GPdir));
        GRed = al_load_bitmap(caminhoGR(GRdir));
        redraw_pacman();
        redraw_map();
        al_rest(0.1);
        pacman = al_load_bitmap(caminho(true,'L'));
        GBlue = al_load_bitmap(caminhoGB(GBdir));
        GOrng = al_load_bitmap(caminhoGO(GOdir));
        GPink = al_load_bitmap(caminhoGP(GPdir));
        GRed = al_load_bitmap(caminhoGR(GRdir));
        redraw_pacman();
        redraw_map();
        al_rest(0.1);
    }
    if(estado== 'R')
    {
        pacman = al_load_bitmap(caminho(false,'R'));
        GBlue = al_load_bitmap(caminhoGB(GBdir));
        GOrng = al_load_bitmap(caminhoGO(GOdir));
        GPink = al_load_bitmap(caminhoGP(GPdir));
        GRed = al_load_bitmap(caminhoGR(GRdir));
        redraw_pacman();
        redraw_map();
        al_rest(0.1);
        pacman = al_load_bitmap(caminho(true,'R'));
        GBlue = al_load_bitmap(caminhoGB(GBdir));
        GOrng = al_load_bitmap(caminhoGO(GOdir));
        GPink = al_load_bitmap(caminhoGP(GPdir));
        GRed = al_load_bitmap(caminhoGR(GRdir));
        redraw_pacman();
        redraw_map();
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

    bool sair = false;

    while(!sair)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);


        if(ev.type == ALLEGRO_EVENT_TIMER)
        {
            if(GOposx == posx && GOposy == posy)
            {

                game_over();
            }
            else if(GBposx == posx && GBposy == posy)
                {


                    game_over();
                }
            else if(GRposx == posx && GRposy == posy)
                {


                    game_over();
                }
            else if(GPposx == posx && GPposy == posy)
                {

                    game_over();
                }

            if(key[KEY_UP] && MAPA[i-1][j] != '1')
            {
                if(MAPA[i][j]=='0'){
                    qscore++;
                    MAPA[i][j]='2';
                }

                pac_estado = 'U';// pacmanU = pacman Up
                i--;
                posy = i*q;



                //animacao(pac_estado,posx,posy,i,j);
            }

            else if(key[KEY_DOWN] && MAPA[i+1][j] != '1')
            {
                if(MAPA[i][j]=='0'){
                    MAPA[i][j]='2';
                    qscore++;
                }

                pac_estado = 'D';
                i++;
                posy = i*q;

            }

            else if(key[KEY_LEFT] && MAPA[i][j-1] != '1')
            {
                if(MAPA[i][j]=='0'){
                    MAPA[i][j]='2';
                    qscore++;
                }
                pac_estado = 'L';
                j--;
                posx = j*q;

            }

            else if(key[KEY_RIGHT] && MAPA[i][j+1] != '1')
            {
                if(MAPA[i][j]=='0'){
                    MAPA[i][j]='2';
                    qscore++;
                }
                pac_estado = 'R';
                j++;
                posx = j*q;
            }

            if(GOposx == posx && GOposy == posy)
            {

                game_over();
            }
            else if(GBposx == posx && GBposy == posy)
                {


                    game_over();
                }
            else if(GRposx == posx && GRposy == posy)
                {


                    game_over();
                }
            else if(GPposx == posx && GPposy == posy)
                {

                    game_over();
                }



            GBdir = GBmove(GBi, GBj);
            if(GBdir == 'U' && MAPA[GBi-1][GBj] != '1')
            {
                GBdir = 'U';// pacmanU = pacman Up
                GBi--;
                GBposy = GBi*q;



                //animacao(pac_estado,posx,posy,i,j);
            }

            else if(GBdir == 'D' && MAPA[GBi+1][GBj] != '1')
            {

                GBdir = 'D';
                GBi++;
                GBposy = GBi*q;

            }

            else if(GBdir == 'L' && MAPA[GBi][GBj-1] != '1')
            {
                GBdir = 'L';
                GBj--;
                GBposx = GBj*q;

            }

            else if(GBdir == 'R' && MAPA[GBi][GBj+1] != '1')
            {
                GBdir = 'R';
                GBj++;
                GBposx = GBj*q;
            }

            GOdir = GOmove(GOi, GOj);
            if(GOdir == 'U' && MAPA[GOi-1][GOj] != '1')
            {
                GOdir = 'U';// pacmanU = pacman Up
                GOi--;
                GOposy = GOi*q;



                //animacao(pac_estado,posx,posy,i,j);
            }

            else if(GOdir == 'D' && MAPA[GOi+1][GOj] != '1')
            {

                GOdir = 'D';
                GOi++;
                GOposy = GOi*q;

            }

            else if(GOdir == 'L' && MAPA[GOi][GOj-1] != '1')
            {
                GOdir = 'L';
                GOj--;
                GOposx = GOj*q;

            }

            else if(GOdir == 'R' && MAPA[GOi][GOj+1] != '1')
            {
                GOdir = 'R';
                GOj++;
                GOposx = GOj*q;
            }

            GPdir = GPmove(GPi, GPj);
            if(GPdir == 'U' && MAPA[GPi-1][GPj] != '1')
            {
                GPdir = 'U';// pacmanU = pacman Up
                GPi--;
                GPposy = GPi*q;



                //animacao(pac_estado,posx,posy,i,j);
            }

            else if(GPdir == 'D' && MAPA[GPi+1][GPj] != '1')
            {

                GPdir = 'D';
                GPi++;
                GPposy = GPi*q;

            }

            else if(GPdir == 'L' && MAPA[GPi][GPj-1] != '1')
            {
                GPdir = 'L';
                GPj--;
                GPposx = GPj*q;

            }

            else if(GPdir == 'R' && MAPA[GPi][GPj+1] != '1')
            {
                GPdir = 'R';
                GPj++;
                GPposx = GPj*q;
            }

            GRdir = GRmove(GRi, GRj);
            if(GRdir == 'U' && MAPA[GRi-1][GRj] != '1')
            {
                GRdir = 'U';// pacmanU = pacman Up
                GRi--;
                GRposy = GRi*q;



                //animacao(pac_estado,posx,posy,i,j);
            }

            else if(GRdir == 'D' && MAPA[GRi+1][GRj] != '1')
            {

                GRdir = 'D';
                GRi++;
                GRposy = GRi*q;

            }

            else if(GRdir == 'L' && MAPA[GRi][GRj-1] != '1')
            {
                GRdir = 'L';
                GRj--;
                GRposx = GRj*q;

            }

            else if(GRdir == 'R' && MAPA[GRi][GRj+1] != '1')
            {
                GRdir = 'R';
                GRj++;
                GRposx = GRj*q;
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

            if(MAPA[i][j] != '0')
                al_play_sample(sample_ghosts, 1.0, 0.0, 1.05, ALLEGRO_PLAYMODE_ONCE, NULL);


            animacao(pac_estado);



        }
    }

    destroy_all();


    return 0;
}




