#include <stdio.h>
#include <stdint.h>

typedef uint8_t Byte;

// ������ �и�Ƽ�� ����ϴ� �Լ�
int calculate_parity(uint8_t x) {
    x ^= x >> 4; // ���� 4��Ʈ�� ���� 4��Ʈ�� XOR
    x ^= x >> 2; // ���� 2��Ʈ�� ���� 2��Ʈ�� XOR
    x ^= x >> 1; // ���� 1��Ʈ�� ���� 1��Ʈ�� XOR
    return x & 1; // ������ ��Ʈ�� �и�Ƽ ��
}

// GF(2^8)���� ���� ����
Byte gf256_multiply(Byte a, Byte b) {
    Byte result = 0;
    while (b) {
        if (b & 1)
            result ^= a;
        a = (a << 1) ^ ((a & 0x80) ? 0x1B : 0); // AES�� ���� ���׽� x^8 + x^4 + x^3 + x + 1
        b >>= 1;
    }
    return result;
}
// GF(2^8)���� ������ ����ϴ� �Լ�
Byte gf256_inverse(Byte x) {
    Byte result = 1, power = x;
    for (int i = 1; i < 255; i++) {
        result ^= power;
        power = gf256_multiply(power, x);
    }
    return result;
}

// Affine ��ȯ ����
Byte affine_transform(Byte x) {
    const Byte M[8] = { 0xF1, 0xE3, 0xC7, 0x8F, 0x1F, 0x3E, 0x7C, 0xF8 }; // ���� ���
    Byte result = 0;
    for (int i = 0; i < 8; i++) {
        Byte bit = x & M[i];
        result ^= calculate_parity(bit); // ��Ʈ �и�Ƽ ���
    }
    return result ^ 0x63; // ��� �߰� (AES�� ����)
}

// ARIA�� S-box �ʱ�ȭ
void generate_aria_sbox(Byte S1[256], Byte S2[256]) {
    // AES�� ������ ������� ���� ��ȯ�� ����
    int i;
    for (i = 0; i < 256; i++) {
        // S1: AES S-box�� ������ ���
        Byte x = (Byte)i;
        Byte inv = (x == 0) ? 0 : gf256_inverse(x); // GF(2^8)���� ���� ���
        S1[i] = affine_transform(inv);

        // S2: S1�� ���� �迭 (ARIA�� �� ��° S-box)
        S2[i] = affine_transform(~inv); // ���ϵ� ���� ���� ������ ��ȯ ����
    }
}

// �׽�Ʈ �Լ�
void print_sbox(const char* name, const Byte sbox[256]) {
    printf("%s:\n", name);
    for (int i = 0; i < 256; i++) {
        printf("0x%02X%s", sbox[i], (i % 16 == 15 ? "\n" : ", "));
    }
}

int main() {
    Byte S1[256], S2[256];

    // ARIA S-box ����
    generate_aria_sbox(S1, S2);

    // ��� ���
    print_sbox("S-box S1", S1);
    print_sbox("S-box S2", S2);

    return 0;
}