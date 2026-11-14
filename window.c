#include <allegro5/allegro.h>
#include <stdbool.h>
#include "window.h"

int create_window(int width, int height) {

    // criar a janela
    if (!al_init()) {
        return 1;
    }

    ALLEGRO_DISPLAY *display = al_create_display(width, height);
    if (!display) {
        return 1;
    }

    // fechar a janela quando clicar no botao de fechar
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));

    // pintar a janela para ela nao ficar vazia
    al_clear_to_color(al_map_rgb(30, 120, 200));
    al_flip_display();

    bool running = true;

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }
    }

    al_destroy_event_queue(queue);
    al_destroy_display(display);
    return 0;
}