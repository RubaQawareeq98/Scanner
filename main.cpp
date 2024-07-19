#include "scanner.h"
#include "fd.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FileDescriptor fd(argv[1]);
    if (!fd.IsOpen()) {
        printf("Can't open %s\n", argv[1]);
        return 1;
    }

    SCANNER scanner(&fd);
    TOKEN *token;
    do {
        token = scanner.Scan();
        scanner.printToken(token);
        delete token;
    } while (token->type != lx_eof);

    return 0;
}
