#include "main.h"

string hash_function(string &key)
{
    // Pagrindiniai konstantos algoritmui (pirmieji skaičiai)
    const uint64_t PRIME1 = 0x9E3779B185EBCA87ULL; // mums reikia unikalių ir didelių skaičių, naudojame hex
    const uint64_t PRIME2 = 0xC2B2AE3D27D4EB4FULL; // ull - unsigned long long (nes gali būti labai dideli skaičiai)
    const uint64_t PRIME3 = 0x165667B19E3779F9ULL;
    const uint64_t PRIME4 = 0x85EBCA77C2B2AE63ULL;
    
    // Inicializuojame keturias hash reikšmes 256 bitams (4 * 64 = 256)
    uint64_t hash1 = PRIME1;
    uint64_t hash2 = PRIME2; 
    uint64_t hash3 = PRIME3;
    uint64_t hash4 = PRIME4;
    
    // Pirmasis etapas: apdorojame kiekvieną baitą/simbolį su visomis keturiomis hash reikšmėmis
    for (size_t i = 0; i < key.length(); i++) {
        uint64_t byte_val = static_cast<uint64_t>(key[i]); // paimame kiekvieną simbolį iš string ir paverčiame į skaičių
        
        // Avalanche effect: kiekvienas baitas paveiks visus bitus visose hash reikšmėse
        hash1 ^= byte_val; // XOR operacija - kiekvienas bitas pakeičiamas, jei baito atitinkamas bitas yra 1
        hash1 *= PRIME2;
        hash1 ^= hash1 >> 33; // maišymas dešinėn (>>) - hash1 bitus pastumiame 33 pozicijomis į dešinę 
        hash1 *= PRIME3;
        hash1 ^= hash1 >> 29;
        
        hash2 ^= byte_val * PRIME1;
        hash2 *= PRIME3;
        hash2 ^= hash2 >> 31;
        hash2 *= PRIME4;
        hash2 ^= hash2 >> 27;
        
        hash3 ^= byte_val * PRIME2;
        hash3 *= PRIME4;
        hash3 ^= hash3 >> 35;
        hash3 *= PRIME1;
        hash3 ^= hash3 >> 23;
        
        hash4 ^= byte_val * PRIME3;
        hash4 *= PRIME1;
        hash4 ^= hash4 >> 37;
        hash4 *= PRIME2;
        hash4 ^= hash4 >> 25;
        
        // Papildomas maišymas su pozicija (deterministinis) visoms hash reikšmėms
        hash1 ^= (i + 1) * PRIME1;
        hash2 ^= (i + 1) * PRIME2;
        hash3 ^= (i + 1) * PRIME3;
        hash4 ^= (i + 1) * PRIME4;
        
        // Bit rotation skirtingais kiekiais kiekvienai hash reikšmei
        hash1 = ((hash1 << 13) | (hash1 >> 51));
        hash2 = ((hash2 << 17) | (hash2 >> 47));
        hash3 = ((hash3 << 19) | (hash3 >> 45));
        hash4 = ((hash4 << 23) | (hash4 >> 41));
    }
    
    // Antrasis etapas: finalizavimas su papildomu maišymu visoms hash reikšmėms
    hash1 ^= key.length() * PRIME2;
    hash1 *= PRIME1;
    hash1 ^= hash1 >> 33;
    hash1 *= PRIME3;
    hash1 ^= hash1 >> 29;
    
    hash2 ^= key.length() * PRIME3;
    hash2 *= PRIME2;
    hash2 ^= hash2 >> 31;
    hash2 *= PRIME4;
    hash2 ^= hash2 >> 27;
    
    hash3 ^= key.length() * PRIME4;
    hash3 *= PRIME3;
    hash3 ^= hash3 >> 35;
    hash3 *= PRIME1;
    hash3 ^= hash3 >> 25;
    
    hash4 ^= key.length() * PRIME1;
    hash4 *= PRIME4;
    hash4 ^= hash4 >> 37;
    hash4 *= PRIME2;
    hash4 ^= hash4 >> 23;
    
    // Trečiasis etapas: konvertuojame į hexadecimalų stringą (4 * 16 = 64 simboliai)
    char hex_chars[] = "0123456789abcdef";
    string hash_value;
    hash_value.reserve(64); // 256 bit = 64 hex simboliai
    
    // Konvertuojame kiekvieną iš keturių 64-bit reikšmių į 16 hex simbolių
    uint64_t hash_values[] = {hash1, hash2, hash3, hash4};
    
    for (int hash_idx = 0; hash_idx < 4; hash_idx++) {
        for (int i = 15; i >= 0; i--) {
            hash_value += hex_chars[(hash_values[hash_idx] >> (i * 4)) & 0xF];
        }
    }
    
    return hash_value;
}