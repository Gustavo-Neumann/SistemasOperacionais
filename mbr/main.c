#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint8_t status;
    uint8_t chs_first[3];
    uint8_t type;
    uint8_t chs_last[3];
    uint32_t lba_first;
    uint32_t sectors;
} EntradaParticao;

void imprimir_info_particao(int indice, EntradaParticao *particao) {
    printf("Partição %d: \n", indice + 1);
    printf("  Status: %s\n", (particao->status == 0x80) ? "Ativa" : "Inativa");
    printf("  Tipo: 0x%02X\n", particao->type);
    printf("  LBA inicial: %u\n", particao->lba_first);
    printf("  Setores: %u\n", particao->sectors);
    printf("  Tamanho aproximado: %.2f MB\n", (particao->sectors * 512.0) / (1024 * 1024));
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_mbr>\n", argv[0]);
        return 1;
    }

    FILE *arquivo = fopen(argv[1], "rb");
    if (!arquivo) {
        perror("Erro ao abrir arquivo");
        return 1;
    }

    uint8_t mbr[512];
    
    if (fread(mbr, 1, 512, arquivo) != 512) {
        perror("Erro ao ler o arquivo");
        fclose(arquivo);
        return 1;
    }

    fclose(arquivo);

    if (mbr[510] != 0x55 || mbr[511] != 0xAA) {
        fprintf(stderr, "Assinatura de boot inválida. O arquivo não parece ser uma MBR.\n");
        return 1;
    }

    EntradaParticao *particoes = (EntradaParticao *)(mbr + 446);

    for (int i = 0; i < 4; i++) {
        if (particoes[i].type != 0) {
            imprimir_info_particao(i, &particoes[i]);
        }
    }

    return 0;
}
