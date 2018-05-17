#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/md5.h>

static unsigned char attackDigest[16];
static unsigned char testDigest[16];
static const char alphabet[] =
"abcdefghijklmnopqrstuvwxyz"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789";

static const int alphabetSize = sizeof(alphabet) - 1;

/** khai bao cac ham can thiet de vet can */

void changeStrToDigestMd5(const char *str, unsigned char *digest);
void changeHexToDigest(const char *str, unsigned char *digest);
char matches(const unsigned char *s1, const unsigned char *s2);
void bruteForce(int process_size, int len, int rank);
void mapNumpassToStr(int num_pass, int len, char* str_pass);

/** bien toan cuc rank, size va attack */
int rank, size;
char attack[33];
int main(int argc, char *argv[])
{
    char hostname[50];
   
    int max_len_pass;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    gethostname(hostname,50);

    if(argc !=3) return -1;

    sscanf(argv[1], "%s", attack);
    sscanf(argv[2], "%d", &max_len_pass);
    // chuyen doi dau vao hexa sang digest
    changeHexToDigest(attack, attackDigest);
    printf("%s ;",hostname);
    bruteForce(size, max_len_pass, rank);
    MPI_Finalize();
    return 0;
    
}

/** kiem tra 2 ma hash co bang nhau hay khong? */
char matches(const unsigned char *s1, const unsigned char *s2)
{
    if (strncmp((char *)s1, (char *)s2, 16) == 0) {
        return 1;
    }

    return 0;
}

/** chuyen doi chars* sang digest md5 */
void changeStrToDigestMd5(const char *str, unsigned char *digest)
{
    MD5_CTX c;
    MD5_Init(&c);
    MD5_Update(&c, str, strlen(str));
    MD5_Final(digest, &c);
}

/** chuyn doi ma hex sang digest*/
void changeHexToDigest(const char *str, unsigned char * digest)
{
    unsigned char t = 0;
    char C[2];
    int i;
    for (i = 0; i < 32; ++i) {
        C[0] = str[i];
        C[1] = '\0';
        int num = strtol(C, NULL, 16);
        if (i % 2 == 1) {
            t += num;
            digest[(i-1) / 2] = t;
            t = 0;
        } else {
            t+= num * 16;
        }
    }
}

/** chuyen doi thu tu cua mat khau sang dang string*/
void mapNumpassToStr(int num_pass, int len, char* str_pass){
    int i = len - 1;
    int j;
    int index;
    for(j = 0; j < len; j ++){
        str_pass[j] = 'a';
    }
    str_pass[len] = '\0';
    while(1){
        if (num_pass > 0){
             index = num_pass % alphabetSize;
             str_pass[i] = alphabet[index];
             i--;
             num_pass = num_pass / alphabetSize;
        }else {
             break;
        }
    }

}


/** tan cong vet can*/
void bruteForce(int process_size, int len, int rank){
    long from, to;
    long total_pass = pow(alphabetSize, len);
    from = rank * total_pass / process_size;
    to = (rank + 1 ) * total_pass / process_size; 
    printf("Rank : %d; bat dau tu: %ld; ", rank, from);
    printf("toi: %ld \n",to);
    long i;
    char str_pass[len];
    for(i = from; i < to; i ++){
        mapNumpassToStr(i, len, str_pass);
        changeStrToDigestMd5(str_pass, testDigest);
        if(matches(testDigest, attackDigest) == 1){
            printf("Ket qua tan cong vet can: %s (%s)\n",str_pass, attack);
            break;
        }
    } 
}