#include <stdio.h>
#include <stdint.h>

typedef uint8_t Byte;

// 숫자의 패리티를 계산하는 함수
int calculate_parity(uint8_t x) {
    x ^= x >> 4; // 상위 4비트와 하위 4비트를 XOR
    x ^= x >> 2; // 상위 2비트와 하위 2비트를 XOR
    x ^= x >> 1; // 상위 1비트와 하위 1비트를 XOR
    return x & 1; // 최하위 비트가 패리티 값
}

// GF(2^8)에서 곱셈 수행
Byte gf256_multiply(Byte a, Byte b) {
    Byte result = 0;
    while (b) {
        if (b & 1)
            result ^= a;
        a = (a << 1) ^ ((a & 0x80) ? 0x1B : 0); // AES의 고정 다항식 x^8 + x^4 + x^3 + x + 1
        b >>= 1;
    }
    return result;
}
// GF(2^8)에서 역원을 계산하는 함수
Byte gf256_inverse(Byte x) {
    Byte result = 1, power = x;
    for (int i = 1; i < 255; i++) {
        result ^= power;
        power = gf256_multiply(power, x);
    }
    return result;
}

// Affine 변환 수행
Byte affine_transform(Byte x) {
    const Byte M[8] = { 0xF1, 0xE3, 0xC7, 0x8F, 0x1F, 0x3E, 0x7C, 0xF8 }; // 예시 행렬
    Byte result = 0;
    for (int i = 0; i < 8; i++) {
        Byte bit = x & M[i];
        result ^= calculate_parity(bit); // 비트 패리티 계산
    }
    return result ^ 0x63; // 상수 추가 (AES와 유사)
}

// ARIA의 S-box 초기화
void generate_aria_sbox(Byte S1[256], Byte S2[256]) {
    // AES와 유사한 방식으로 비선형 변환을 수행
    int i;
    for (i = 0; i < 256; i++) {
        // S1: AES S-box와 동일한 방식
        Byte x = (Byte)i;
        Byte inv = (x == 0) ? 0 : gf256_inverse(x); // GF(2^8)에서 역원 계산
        S1[i] = affine_transform(inv);

        // S2: S1의 역순 배열 (ARIA의 두 번째 S-box)
        S2[i] = affine_transform(~inv); // 보완된 값에 대해 동일한 변환 적용
    }
}

// 테스트 함수
void print_sbox(const char* name, const Byte sbox[256]) {
    printf("%s:\n", name);
    for (int i = 0; i < 256; i++) {
        printf("0x%02X%s", sbox[i], (i % 16 == 15 ? "\n" : ", "));
    }
}

int main() {
    Byte S1[256], S2[256];

    // ARIA S-box 생성
    generate_aria_sbox(S1, S2);

    // 결과 출력
    print_sbox("S-box S1", S1);
    print_sbox("S-box S2", S2);

    return 0;
}