section .data
    message db "Hello", 0
    message2 db "Hell", 0
    inttest db 123
    lr db 10, 0
    file db "test.asm", 0

section .bss
    read_buf resb 32
    write_buf resb 32
    buffer resb 32

section .text
    global _start
    
    ; my_hello_world()
    my_hello_world:
        MOV rdi, message        ; 2eme argument d'un syscall
        CALL my_write
        RET                     ; return

    ; my_read(fd)
    my_read:
        MOV rax, 0                  ; syscall de read
        MOV rsi, read_buf           ; buf de read
        MOV rdx, 32                 ; len de buf
        SYSCALL
        RET                         ; return

    ; my_write(message, fd, int) == \n si int = 1
    my_write:
        PUSH rbp
        PUSH rbx
        MOV rbp, rdx
        MOV rbx, rsi
        MOV rsi, rdi                ; le message passe en parametre
        CALL my_strlen              ; rdi encore intact ici
        MOV rdx, rax
        MOV rdi, rbx                ; fd
        MOV rax, 1                  ; syscall de write
        SYSCALL
        CMP rbp, 1           ; check le param pour \n
        JNE my_write_end
        MOV rdi, rbx
        MOV rsi, lr
        MOV rdx, 1
        MOV rax, 1
        SYSCALL
        my_write_end:
            POP rbx
            POP rbp
            RET                     ; return

    ; my_strlen(char *)
    my_strlen:
        MOV rcx, 0                      ; compteur a 0
        start_while:                    ; label debut de boucle
            CMP byte [rdi + rcx], 0     ; regarde le char de rdi + off
            JE end_while                ; si il est nul go label end
            INC rcx                     ; on ajoute 1 au off
            JMP start_while             ; on retourne au debut
        end_while:                      ; label de fin
            MOV rax, rcx                ; ajoute la valeur trouve dans rax
            RET                         ; return
    
    ; my_strcpy(char *dst, char *src, int size)
    my_strlcpy:
        MOV rcx, 0

        CMP rdi, 0
        JE end_strlcpy
        CMP rsi, 0
        JE end_strlcpy
        CMP rdx, 0
        JBE end_strlcpy

        start_while_strlcpy:
            DEC rdx
            CMP rcx, rdx
            JAE end_strlcpy
            INC rdx
            CMP byte [rsi + rcx], 0
            JE end_strlcpy

            MOV al, [rsi + rcx]
            MOV [rdi + rcx], al
            
            INC rcx
            MOV r8b, byte [rsi + rcx]
            SUB al, r8b
            JNE end_strlcpy
            
            CMP byte [rdi + rcx], 0
            JE end_strlcpy

            INC rcx
            JMP end_strlcpy
        end_strlcpy:
            RET

    ;rev_string(char *)
    rev_string:
        CALL my_strlen
        MOV rcx, 0
        LEA rsi, [rdi + rax - 2]
        MOV r10, rsi
        SUB r10, rcx

        start_while_rev_string:
            MOV r8, rdi
            ADD r8, rcx
            CMP r10, r8
            JB end_while_rev_string

            MOV r8b, byte [rdi + rcx]
            MOV r9b, byte [r10]

            MOV byte [rdi + rcx], r9b
            MOV byte [r10], r8b

            INC rcx
            MOV r10, rsi
            SUB r10, rcx
            JMP start_while_rev_string
        end_while_rev_string:
            RET

    ;my_itoa(int)
    my_itoa:
        MOV rcx, 0
        MOV rax, rdi

        start_while_itoa:
            CMP rax, 10
            JB end_while_itoa

            MOV r8, 10
            MOV rdx, 0
            DIV r8
            ADD dl, '0'
            MOV byte [buffer + rcx], dl

            INC rcx
            JMP start_while_itoa
        end_while_itoa:
            MOV dl, al
            ADD dl, '0'
            MOV byte [buffer + rcx], dl
            INC rcx
            MOV byte [buffer + rcx], 10
            INC rcx
            MOV byte [buffer + rcx], 0
            MOV rdi, buffer
            CALL rev_string
            RET

    ;my_atoi
    my_atoi:
        MOV rcx, 0
        MOV rax, 0
        PUSH rbx

        start_while_atoi:
            CMP byte [rdi + rcx], 0
            JE end_while_atoi
            CMP byte [rdi + rcx], 10
            JE end_while_atoi

            MOV dl, byte [rdi + rcx]
            SUB dl, '0'

            MOV bl, dl
            MOV r8, 10
            MUL r8
            MOV dl, bl
            MOVZX rdx, dl
            ADD rax, rdx

            INC rcx
            JMP start_while_atoi
        end_while_atoi:
            POP rbx
            RET

    ;my_strcat(char *dst, char *src)
    my_strcat:
        MOV rbx, rdi   ; rbx = dst
        MOV rbp, rsi   ; rbp = src

        MOV rdi, rbx
        CALL my_strlen ; rax = strlen(dst)
        ADD rbx, rax   ; copie le ret dans rdx
        
        MOV rdi, rbp
        CALL my_strlen ; rax = strlen(src)

        MOV rdi, rbx
        MOV rsi, rbp
        INC rax
        MOV rdx, rax
        CALL my_strlcpy
        RET

    ;my_cat(filename)
    ; on doit open un fichier
    ; le lire et ecrire quand le buff est plein ou on a atteind la fin
    ; le fermer quand on a termine
    my_cat:
        MOV rdi, rdi
        MOV rsi, 0
        MOV rdx, 0
        MOV rax, 2                      ; syscall de open
        SYSCALL                         ; return le fd j'imagine
        MOV rbx, rax                    ; stocke le fd du file
        CMP rbx, 0                      ; si open a fail
        JL   end_my_cat

        start_my_cat:
            MOV rdi, rbx                ; met le fd pour read
            CALL my_read
            MOV r12, rax                ; met le nbr de char lu dans r12
            CMP r12, 0                  ; si read = 0 return
            JLE end_my_cat   
            MOV rdi, read_buf
            MOV rsi, 1
            MOV rdx, 0
            CALL my_write               ; write le buf

            JMP start_my_cat
        end_my_cat:
            RET




_start:
;---------------------- test de strlcpy -----------------------;
;    MOV rdi, message                                          ;
;    CALL my_strlen      ;=> taille de message                 ;
;    INC rax                                                   ;
;    MOV rdi, buffer                                           ;
;    MOV rsi, message                                          ;
;    MOV rdx, rax                                              ;
;    CALL my_strlcpy     ;=> copie de message dans buffer      ;
;    MOV rdi, buffer                                           ;
;    MOV rsi, 1                                                ;
;    CALL my_write      ;=> affiche buffer                     ;
;--------------------------------------------------------------;
;---------------------- test de strcmp ------------------------;
;    MOV rdi, message                                          ;
;    MOV rsi, message2                                         ;
;    MOV rdx, 5                                                ;
;    CALL my_strcmp                                            ;
;--------------------------------------------------------------;
;--------------------- test de atoi + itoa --------------------;
;    CALL my_read                                              ;
;    MOV rdi, read_buf                                         ;
;    CALL my_atoi                                              ;
;    MOV rdi, rax                                              ;
;    CALL my_itoa                                              ;
;    MOV rdi, buffer                                           ;
;    MOV rsi, 1                                                ;
;    CALL my_write                                             ;
;--------------------------------------------------------------;
;--------------------- test de strcat -------------------------;
;   MOV rdi, buffer                                            ;
;   MOV rsi, message                                           ;
;   CALL my_strcat                                             ;
;   MOV rdi, buffer                                            ;
;   MOV rsi, 1                                                 ;
;   CALL my_write                                              ;
;--------------------------------------------------------------;
        MOV rdi, 1
        CALL my_read
        DEC rax
        MOV byte [read_buf + rax], 0
        MOV rdi, read_buf
        MOV rsi, 1
        MOV rdx, 1
        CALL my_write
        MOV rdi, read_buf
        CALL my_cat

    MOV rdi, rax
    MOV rax, 60                 ; numero du syscall exit
    SYSCALL                     ; exec exit 0