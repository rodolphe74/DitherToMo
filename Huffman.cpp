
#include "Huffman.h"

// #define min(a, b) (a < b ? a : b)
//
// typedef unsigned char byte;
// typedef int16_t INT;
//
// const INT search_buf    = 1024;
// const INT lookahead_buf = 100; // limited to 255
//
// typedef struct tuple {
//     unsigned short backward;
//     byte len;
//     byte symbol;
// } tuple;
//
// typedef struct dynamic_list {
//     tuple* buf;
//     INT    capacity;
//     INT    len;
// } dynamic_list;

void free_list(dynamic_list list) {
	free(list.buf);
}

dynamic_list inicialize(INT capacity) {
    dynamic_list aux;

    aux.capacity  = capacity;
    aux.buf       = (TUPLE*)malloc(sizeof(TUPLE) * capacity);

    if(aux.buf == NULL) {
        printf("Erro de alocacao\n");
        exit(0);
    }

    aux.len = 0;
    return aux;
}

TUPLE make_tuple(unsigned short arg1, BYTE arg2, BYTE arg3) {
    TUPLE ret;
    ret.backward = arg1;
    ret.len      = arg2;
    ret.symbol   = arg3;
    return ret;
}

void insert(dynamic_list* list, TUPLE arg) {
    if(list->len < list->capacity) {
        list->buf[list->len] = arg;
    } else {
        // double capacity
        TUPLE* resized_buf = (TUPLE*)malloc(sizeof(TUPLE) * list->capacity * 2);

        // copy data to new buff
        for(INT i = 0; i < list->capacity; i++) {
            resized_buf[i] = list->buf[i];
        }

        // free old data buf
        free(list->buf);

        // updating new capacity
        list->capacity       = list->capacity * 2;
        list->buf            = resized_buf;
		list->buf[list->len] = arg;
    }

    list->len++;
}

void list_tuples(dynamic_list list) {
    for(INT i = 0; i < list.len; i++) {
        printf("(%d, %c, %c) ", list.buf[i].backward, list.buf[i].len, list.buf[i].symbol);
    }
    printf("\n");
}

void write_binary(dynamic_list list, const char* out) {
    FILE* fp = fopen(out, "wb");

    if(fp == NULL) {
        printf("Erro na alocacao de memoria\n");
        return;
    }

    fwrite(list.buf, sizeof(TUPLE) * list.len, 1, fp);
    fclose(fp);
}

INT read_binary(BYTE** buf, const char* in) {
    FILE* fp = fopen(in, "rb");

    if(fp == NULL) {
        printf("Erro na alocacao de memoria\n");
        return -1;
    }

    fseek(fp, 0L, SEEK_END);
    INT len = (INT) ftell(fp);
    rewind(fp);

    *buf = (BYTE*)malloc(sizeof(BYTE) * len);

    fread(*buf, len, 1, fp);
    fclose(fp);
    return len;
}

INT compression(BYTE buf[], INT len, const char* out) {
    dynamic_list list;

    list = inicialize(1024);

    insert(&list, make_tuple(0, 0, buf[0]));

    for(INT i = 1; i < len; i++) {
        TUPLE temp = make_tuple(0, 0, buf[i]);
        for(INT j = i-1; j >= 0 && j >= i-search_buf; j--) {
            INT cx = j;
            for(INT p = i; p < min(i+lookahead_buf, len) && cx < i; p++) {
                if(buf[cx] == buf[p]) {
                    if(temp.len < p-i+1) {
                        temp = make_tuple(i-j, p-i+1, buf[p+1 >= len ? p : p+1]);
                    }
                    cx++;
                } else {
                    break;
                }
            }
        }

        insert(&list, temp);
        i += temp.len ? temp.len : 0;
    }

    write_binary(list, out);
    free_list(list);
    return list.len * sizeof(TUPLE);
}



INT compressionv(const std::vector<uint8_t> &in, std::vector<uint8_t> &out)
{
    BYTE *buf = new BYTE[in.size()];
    int len = in.size();
    printf("len:%d\n", len);
    int i = 0;
    for (auto c = in.begin(); c != in.end(); c++) {
        buf[i++] = *c;
    }
    dynamic_list list;

    list = inicialize(1024);

    insert(&list, make_tuple(0, 0, buf[0]));

    for(INT i = 1; i < len; i++) {
        TUPLE temp = make_tuple(0, 0, buf[i]);
        for(INT j = i-1; j >= 0 && j >= i-search_buf; j--) {
            INT cx = j;
            for(INT p = i; p < min(i+lookahead_buf, len) && cx < i; p++) {
                if(buf[cx] == buf[p]) {
                    if(temp.len < p-i+1) {
                        temp = make_tuple(i-j, p-i+1, buf[p+1 >= len ? p : p+1]);
                    }
                    cx++;
                } else {
                    break;
                }
            }
        }

        insert(&list, temp);
        i += temp.len ? temp.len : 0;
    }

    // fwrite(list.buf, sizeof(TUPLE) * list.len, 1, fp);

    TUPLE * tuples = list.buf;
    for (int i = 0; i < list.len; i++) {
        char *t = (char *) &tuples[i];
        for (int j = 0; j < sizeof(TUPLE); j++) {
            out.push_back(*t);
        }
    }

    printf("vector size:%d\n", (int) out.size());

    free_list(list);
    return list.len * sizeof(TUPLE);
}





void free_memory(decompressed_bytes arr) {
    free(arr.buf);
}

decompressed_bytes inicialized(INT capacity) {
    decompressed_bytes aux;
    aux.buf       = (BYTE*)malloc(sizeof(BYTE) * capacity);
    aux.len       = 0;
    aux.capacity  = capacity;
    return aux;
}



void insert(decompressed_bytes* arr, BYTE data) {
    if(arr->len < arr->capacity) {
        arr->buf[arr->len] = data;
    } else {
        // double capacity
        // byte* resized_buf = (byte*)malloc(sizeof(byte) * arr->capacity * 2);
        printf("increase:%d\n", arr->capacity);
        BYTE* resized_buf = (BYTE*)malloc(sizeof(BYTE) * (arr->capacity + 1024));

        if (resized_buf == NULL) {
            perror("can't extend buffer");
            exit(1);
        }


        // copy data to new buff
        for(UINT i = 0; i < arr->capacity; i++) {
            resized_buf[i] = arr->buf[i];
        }

        // free old data buf
        free(arr->buf);

        // updating new capacity
        // arr->capacity      = arr->capacity * 2;
        arr->capacity      = arr->capacity + 1024;
        arr->buf           = resized_buf;
		arr->buf[arr->len] = data;
    }

    arr->len++;
}




void write_binary(BYTE* buf, INT len, const char* out) {
    FILE* fp = fopen(out, "wb");

    if(fp == NULL) {
        printf("Erro na alocacao de memoria\n");
        exit(0);
    }

    fwrite(buf, len, 1, fp);
    fclose(fp);
}

// len is a multiple of 4
void decompress(BYTE* buf, UINT len, const char* out) {
    decompressed_bytes arr;
    arr = inicialized(1024);
    TUPLE aux;
    for(UINT i = 0; i < len; i += 4) {
        memcpy(&aux, buf+i, sizeof(TUPLE));
        if(aux.len == 0) {
            insert(&arr, aux.symbol);
        } else {
            UINT ax = arr.len;
            for(UINT i = 0; i < aux.len; i++) {
                insert(&arr, arr.buf[ax-aux.backward+i]);
            }

            if(i+4 < len && aux.len > 0)
            	insert(&arr, aux.symbol);
        }
    }


    write_binary(arr.buf, arr.len, out);
    free_memory(arr);
}




int main() {
    BYTE* buf;

    INT n = read_binary(&buf, "./xx_00000.h");

    printf("%d uncompressed bytes\n", n);

    INT compressed_len = compression(buf, n, "saida.out");

    double compressed_rate = 100 - (compressed_len/(double)n) * 100;

    printf("%lf percent smaller\n", compressed_rate);

    BYTE* bufd;
    UINT len = read_binary(&bufd, "saida.out");
    decompress(bufd, len, "xx_00000d.h");


    // std::vector<uint8_t> v,w;
    // for (int i = 0; i < n; i++) {
        // v.push_back(buf[i]);
    // }
    // compressionv(v, w);


    return 0;
}
