/* MIHAESCU Razvan-Andrei - 312CC */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXVAL 255

// structura pentru un pixel
typedef struct rgb {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} rgb;

// structura pentru arborele cuaternar
typedef struct quadtree {
    rgb color;
    struct quadtree* child[4];
    // am folosit si un camp care sa retina, pentru nodurile frunza, dimensiunea
    // imaginii continute de nodul respectiv
    unsigned int size;
} quadtree;

// structura pentru o celula din coada
typedef struct node {
    quadtree* data;
    struct node* next;
} node;

// structura pentru coada
typedef struct queue {
    node* front;
    node* rear;
} queue;

// functia care citeste un fisier de tip PPM
void read_ppm(char* file_path, unsigned int* size, int* maxval, rgb*** image) {
    // deschid fisierul pentru citire
    FILE* file = fopen(file_path, "rb");
    char header[3];
    fscanf(file, "%s", header);
    // citesc dimensiunea imaginii si valoarea maxima a unui pixel
    fscanf(file, "%u %u %d", size, size, maxval);
    // sar peste newline
    fseek(file, 1, SEEK_CUR);
    // aloc memorie pentru matricea de pixeli
    *image = (rgb**)malloc(*size * sizeof(rgb*));
    int i;
    // aloc memorie pentru fiecare linie din matricea de pixeli
    for (i = 0; i < *size; i++) {
        (*image)[i] = (rgb*)malloc(*size * sizeof(rgb));
    }
    int j;
    // citesc fiecare pixel din imagine
    for (i = 0; i < *size; i++) {
        for (j = 0; j < *size; j++) {
            fread(&(*image)[i][j].r, sizeof(unsigned char), 1, file);
            fread(&(*image)[i][j].g, sizeof(unsigned char), 1, file);
            fread(&(*image)[i][j].b, sizeof(unsigned char), 1, file);
        }
    }
    fclose(file);
}

// functia care calculeaza media culorii rosu dintr-o imagine
unsigned long long mean_red(rgb** image, unsigned int size, int x, int y) {
    int i, j;
    unsigned long long red = 0;
    for (i = x; i < x + size; i++) {
        for (j = y; j < y + size; j++) {
            red += (unsigned long long)image[i][j].r;
        }
    }
    red = (unsigned long long)((double)red / (size * size));
    return red;
}

// functia care calculeaza media culorii verde dintr-o imagine
unsigned long long mean_green(rgb** image, unsigned int size, int x, int y) {
    int i, j;
    unsigned long long green = 0;
    for (i = x; i < x + size; i++) {
        for (j = y; j < y + size; j++) {
            green += (unsigned long long)image[i][j].g;
        }
    }
    green = (unsigned long long)((double)green / (size * size));
    return green;
}

// functia care calculeaza media culorii albastru dintr-o imagine
unsigned long long mean_blue(rgb** image, unsigned int size, int x, int y) {
    int i, j;
    unsigned long long blue = 0;
    for (i = x; i < x + size; i++) {
        for (j = y; j < y + size; j++) {
            blue += (unsigned long long)image[i][j].b;
        }
    }
    blue = (unsigned long long)((double)blue / (size * size));
    return blue;
}

// functia care calculeaza media culorii dintr-o imagine
unsigned long long mean_color(rgb** image, unsigned int size, int x, int y,
                              unsigned long long red, unsigned long long green,
                              unsigned long long blue) {
    int i, j;
    unsigned long long color = 0;
    for (i = x; i < x + size; i++) {
        for (j = y; j < y + size; j++) {
            color += ((unsigned long long)image[i][j].r - red) *
                         ((unsigned long long)image[i][j].r - red) +
                     ((unsigned long long)image[i][j].g - green) *
                         ((unsigned long long)image[i][j].g - green) +
                     ((unsigned long long)image[i][j].b - blue) *
                         ((unsigned long long)image[i][j].b - blue);
        }
    }
    color = (unsigned long long)((double)color / (3 * size * size));
    return color;
}

// functia care elibereaza memoria alocata unei imagini
void free_image(rgb** image, unsigned int size) {
    int i;
    for (i = 0; i < size; i++) {
        free(image[i]);
    }
    free(image);
}

// functia care construieste arborele cuaternar
void build_quadtree(quadtree* root, rgb** image, unsigned int size,
                    unsigned int factor) {
    unsigned int i, j;
    unsigned long long red, green, blue, color;
    // daca dimensiunea imaginii este 1, atunci nodul curent este frunza
    if (size == 1) {
        root->color.r = image[0][0].r;
        root->color.g = image[0][0].g;
        root->color.b = image[0][0].b;
        root->size = size;
        for (i = 0; i < 4; i++) {
            root->child[i] = NULL;
        }
    } else {
        // creez cele 4 imagini corespunzatoare celor 4 copii si le aloc memorie
        rgb **image_upper_left, **image_upper_right, **image_lower_left,
            **image_lower_right;
        image_upper_left = (rgb**)malloc(size / 2 * sizeof(rgb*));
        image_upper_right = (rgb**)malloc(size / 2 * sizeof(rgb*));
        image_lower_left = (rgb**)malloc(size / 2 * sizeof(rgb*));
        image_lower_right = (rgb**)malloc(size / 2 * sizeof(rgb*));
        for (i = 0; i < size / 2; i++) {
            image_upper_left[i] = (rgb*)malloc(size / 2 * sizeof(rgb));
            image_upper_right[i] = (rgb*)malloc(size / 2 * sizeof(rgb));
            image_lower_left[i] = (rgb*)malloc(size / 2 * sizeof(rgb));
            image_lower_right[i] = (rgb*)malloc(size / 2 * sizeof(rgb));
        }
        for (i = 0; i < size / 2; i++) {
            for (j = 0; j < size / 2; j++) {
                // copiez pixelii din imaginea initiala in cele 4 imagini
                image_upper_left[i][j].r = image[i][j].r;
                image_upper_left[i][j].g = image[i][j].g;
                image_upper_left[i][j].b = image[i][j].b;
                image_upper_right[i][j].r = image[i][j + size / 2].r;
                image_upper_right[i][j].g = image[i][j + size / 2].g;
                image_upper_right[i][j].b = image[i][j + size / 2].b;
                image_lower_left[i][j].r = image[i + size / 2][j].r;
                image_lower_left[i][j].g = image[i + size / 2][j].g;
                image_lower_left[i][j].b = image[i + size / 2][j].b;
                image_lower_right[i][j].r = image[i + size / 2][j + size / 2].r;
                image_lower_right[i][j].g = image[i + size / 2][j + size / 2].g;
                image_lower_right[i][j].b = image[i + size / 2][j + size / 2].b;
            }
        }
        // calculez media culorilor din imagine
        red = mean_red(image, size, 0, 0);
        green = mean_green(image, size, 0, 0);
        blue = mean_blue(image, size, 0, 0);
        color = mean_color(image, size, 0, 0, red, green, blue);
        // daca media culorilor din imagine este mai mica decat factorul
        if ((unsigned int)color <= factor) {
            // nodul curent este frunza si ii setez campurile corespunzatoare
            // drept culorile medii calculate
            root->color.r = red;
            root->color.g = green;
            root->color.b = blue;
            root->size = size;
            for (i = 0; i < 4; i++) {
                root->child[i] = NULL;
            }
        } else {
            // altfel, nodul curent nu este frunza si apelez recursiv functia cu
            // noile imagini si dimensiuni
            for (i = 0; i < 4; i++) {
                root->child[i] = (quadtree*)malloc(sizeof(quadtree));
            }
            build_quadtree(root->child[0], image_upper_left, size / 2, factor);
            build_quadtree(root->child[1], image_upper_right, size / 2, factor);
            build_quadtree(root->child[2], image_lower_right, size / 2, factor);
            build_quadtree(root->child[3], image_lower_left, size / 2, factor);
        }
        // eliberez memoria alocata celor 4 imagini
        free_image(image_upper_left, size / 2);
        free_image(image_upper_right, size / 2);
        free_image(image_lower_right, size / 2);
        free_image(image_lower_left, size / 2);
    }
}

// functia care verifica daca un nod e frunza
int is_leaf(quadtree* root) {
    if (root->child[0] == NULL && root->child[1] == NULL &&
        root->child[2] == NULL && root->child[3] == NULL) {
        return 1;
    }
    return 0;
}

// functia care numara cate nivele are arborele
int count_levels(quadtree* root, int level) {
    if (root == NULL) {
        // daca ajung la o frunza returnez nivelul
        return level;
    }
    int i, max = 0;
    // altfel, apelez recursiv functia pentru fiecare copil
    for (i = 0; i < 4; i++) {
        // si retin maximul dintre nivelul curent si nivelul copilului
        int temp = count_levels(root->child[i], level + 1);
        if (temp > max) {
            max = temp;
        }
    }
    return max;
}

// functia care numara cate blocuri sunt in arbore
int count_blocks(quadtree* root) {
    if (root == NULL) {
        return 0;
    }
    int i, count = 0;
    for (i = 0; i < 4; i++) {
        count += count_blocks(root->child[i]);
    }
    // daca nodul curent este frunza, numar un bloc
    if (is_leaf(root)) {
        count++;
    }
    return count;
}

// functia care calculeaza dimensiunea maxima a unui bloc
int max_size(quadtree* root) {
    if (root == NULL) {
        return 0;
    }
    // calculez maximul dintre dimensiunea blocului curent si maximul dintre cei
    // 4 copii ai sai (daca exista)
    int i, max = 0;
    for (i = 0; i < 4; i++) {
        int temp = max_size(root->child[i]);
        if (temp > max) {
            max = temp;
        }
    }
    // daca radacina e frunza, verific daca dimensiunea ei e mai mare decat
    // maximul
    if (is_leaf(root)) {
        if (root->size > max) {
            max = root->size;
        }
    }
    return max;
}

// functia care adauga un nod in coada
void enqueue(queue* q, quadtree* root) {
    node* new_node = (node*)malloc(sizeof(node));
    new_node->data = root;
    new_node->next = NULL;
    // daca coada e goala, nodul adaugat devine primul si ultimul
    if (q->front == NULL) {
        q->front = new_node;
        q->rear = new_node;
    } else {
        // altfel il adaug la finalul cozii
        q->rear->next = new_node;
        q->rear = new_node;
    }
}

// functia care scoate un nod din coada
quadtree* dequeue(queue* q) {
    // daca coada e goala, returnez NULL
    if (q->front == NULL) {
        return NULL;
    }
    // altfel, scot primul nod si il returnez
    node* temp = q->front;
    quadtree* data = temp->data;
    q->front = q->front->next;
    free(temp);
    return data;
}

// functia care verifica daca o coada e goala
int is_empty(queue* q) {
    if (q->front == NULL) {
        return 1;
    }
    return 0;
}

// functia care elibereaza memoria alocata unei cozi
void free_queue(queue* q) {
    while (!is_empty(q)) {
        dequeue(q);
    }
    free(q);
}

// functia care creeaza fisierul comprimat
void compress_image(quadtree* root, FILE* f) {
    // creez o coada in care adaug nodurile arborelui
    queue* q = (queue*)malloc(sizeof(queue));
    q->front = NULL;
    q->rear = NULL;
    // execut o parcurgere in latime a arborelui
    enqueue(q, root);
    while (!is_empty(q)) {
        // scot nodul curent din coada
        quadtree* temp = dequeue(q);
        // daca nodul curent e frunza, scriu in fisier tipul nodului si culorile
        if (is_leaf(temp)) {
            unsigned char nod_type = 1;
            fwrite(&nod_type, sizeof(unsigned char), 1, f);
            fwrite(&temp->color.r, sizeof(unsigned char), 1, f);
            fwrite(&temp->color.g, sizeof(unsigned char), 1, f);
            fwrite(&temp->color.b, sizeof(unsigned char), 1, f);
        } else {
            // altfel, scriu in fisier tipul nodului si adaug copiii in coada
            unsigned char nod_type = 0;
            fwrite(&nod_type, sizeof(unsigned char), 1, f);
            int i;
            for (i = 0; i < 4; i++) {
                enqueue(q, temp->child[i]);
            }
        }
    }
    // eliberez memoria alocata cozii
    free_queue(q);
}

// functia care creeaza arborele din fisierul comprimat
void build_quadtree_from_file(quadtree* root, FILE* f) {
    unsigned char nod_type;
    // creez o coada in care adaug nodurile arborelui
    queue* q = (queue*)malloc(sizeof(queue));
    q->front = NULL;
    q->rear = NULL;
    // adaug radacina in coada
    enqueue(q, root);
    // adaug nodurile in arbore pe nivel cu ajutorul cozii
    while (!is_empty(q)) {
        // scot nodul curent din coada
        quadtree* temp = dequeue(q);
        fread(&nod_type, sizeof(unsigned char), 1, f);
        // daca nodul curent e frunza, citesc culorile
        if (nod_type == 1) {
            fread(&temp->color.r, sizeof(unsigned char), 1, f);
            fread(&temp->color.g, sizeof(unsigned char), 1, f);
            fread(&temp->color.b, sizeof(unsigned char), 1, f);
            temp->child[0] = NULL;
            temp->child[1] = NULL;
            temp->child[2] = NULL;
            temp->child[3] = NULL;
        } else {
            // altfel, adaug copiii in coada
            int i;
            for (i = 0; i < 4; i++) {
                temp->child[i] = (quadtree*)malloc(sizeof(quadtree));
                enqueue(q, temp->child[i]);
            }
        }
    }
    // eliberez memoria alocata cozii
    free_queue(q);
}

// functia care elibereaza memoria alocata unui arbore
void decompress_image(quadtree* root, rgb** image, unsigned int size, int x,
                      int y) {
    queue* q = (queue*)malloc(sizeof(queue));
    q->front = NULL;
    q->rear = NULL;
    // execut o parcurgere in latime a arborelui
    enqueue(q, root);
    while (!is_empty(q)) {
        quadtree* temp = dequeue(q);
        if (is_leaf(temp)) {
            int i, j;
            // colorez pixelii din imagine cu culorile nodului curent daca nodul
            // e frunza
            for (i = x; i < x + size; i++) {
                for (j = y; j < y + size; j++) {
                    image[i][j].r = temp->color.r;
                    image[i][j].g = temp->color.g;
                    image[i][j].b = temp->color.b;
                }
            }
        } else {
            // altfel, apelez fuctia recursiv pentru fiecare copil al nodului si
            // imaginile corespunzatoare acestora
            decompress_image(temp->child[0], image, size / 2, x, y);
            decompress_image(temp->child[1], image, size / 2, x, y + size / 2);
            decompress_image(temp->child[2], image, size / 2, x + size / 2,
                             y + size / 2);
            decompress_image(temp->child[3], image, size / 2, x + size / 2, y);
        }
    }
    // eliberez memoria alocata cozii
    free_queue(q);
}

// functia care elibereaza memoria alocata unui arbore
void free_quadtree(quadtree* root) {
    if (root == NULL) {
        return;
    }
    int i;
    for (i = 0; i < 4; i++) {
        free_quadtree(root->child[i]);
    }
    free(root);
}

int main(int argc, char* argv[]) {
    int maxval, i, j;
    unsigned int size, factor;
    rgb** image;
    quadtree* root = (quadtree*)malloc(sizeof(quadtree));
    root->child[0] = NULL;
    root->child[1] = NULL;
    root->child[2] = NULL;
    root->child[3] = NULL;
    // daca primul argument e -c1 sau -c2, citesc imaginea si factorul de
    // compresie
    if (strcmp(argv[1], "-c1") == 0 || strcmp(argv[1], "-c2") == 0) {
        read_ppm(argv[3], &size, &maxval, &image);
        factor = (unsigned int)atoi(argv[2]);
        // creez arborele cuaternar
        build_quadtree(root, image, size, factor);
    }

    // daca primul argument e -c1, afisez numarul de nivele, numarul de blocuri
    // si dimensiunea maxima continuta de un bloc
    if (strcmp(argv[1], "-c1") == 0) {
        FILE* f = fopen(argv[4], "w");
        fprintf(f, "%d\n", count_levels(root, 0));
        fprintf(f, "%d\n", count_blocks(root));
        fprintf(f, "%d\n", max_size(root));
        fclose(f);
    }
    // daca primul argument e -c2
    if (strcmp(argv[1], "-c2") == 0) {
        // deschid fisierul in care voi scrie arborele
        FILE* f = fopen(argv[4], "wb");
        // scriu dimensiunea imaginii
        fwrite(&size, sizeof(unsigned int), 1, f);
        // apelez functia care scrie arborele in fisier
        compress_image(root, f);
        fclose(f);
    }
    // daca primul argument e -d
    if (strcmp(argv[1], "-d") == 0) {
        // deschid fisierul din care voi citi arborele
        FILE* f = fopen(argv[2], "rb");
        // citesc dimensiunea imaginii
        fread(&size, sizeof(unsigned int), 1, f);
        // apelez functia care construieste arborele din fisier
        build_quadtree_from_file(root, f);
        FILE* g = fopen(argv[3], "wb");
        // scriu in fisierul de iesire headerul imaginii, dimensiunea imaginii
        // si valoarea maxima a unui pixel
        fprintf(g, "P6\n");
        fprintf(g, "%u %u\n", size, size);
        fprintf(g, "%d\n", MAXVAL);
        // aloc memorie pentru imagine
        image = (rgb**)malloc(size * sizeof(rgb*));
        for (i = 0; i < size; i++) {
            image[i] = (rgb*)malloc(size * sizeof(rgb));
        }
        // apelez functia care decomprima imaginea
        decompress_image(root, image, size, 0, 0);
        // scriu in fisierul de iesire pixelii imaginii
        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++) {
                fwrite(&image[i][j].r, sizeof(unsigned char), 1, g);
                fwrite(&image[i][j].g, sizeof(unsigned char), 1, g);
                fwrite(&image[i][j].b, sizeof(unsigned char), 1, g);
            }
        }
        fclose(f);
        fclose(g);
    }
    // eliberez memoria alocata arborelui si imaginii
    free_quadtree(root);
    free_image(image, size);
    return 0;
}