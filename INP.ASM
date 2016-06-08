.386p
_TEXT   segment byte public use16 'CODE'
_TEXT   ends
_BSS    segment word public use16  'BSS'
_BSS    ends


DGROUP  group   _BSS
        assume  cs:_TEXT,ds:DGROUP
_TEXT   segment byte public use16 'CODE'
        assume  cs:_TEXT, ds:DGROUP

        assume ds:DGROUP

        public _inp, _inpw, _inpd

_inp    proc

	push    bp                             ; save bp to stack
        mov     bp,sp                          ; bp = sp
        xor     eax,eax
        mov     dx,word ptr [bp + 4]
        in      al,dx
        pop	bp
        ret

_inp    endp


_inpw   proc

	push    bp                             ; save bp to stack
        mov     bp,sp                          ; bp = sp
        xor     eax,eax
        mov     dx,word ptr [bp + 4]
        in      ax,dx
        pop	bp
        ret

_inpw   endp

_inpd   proc

	push    bp                             ; save bp to stack
        mov     bp,sp                          ; bp = sp
        xor     eax,eax
        mov     dx,word ptr [bp + 4]
        in      eax,dx
        mov     edx, eax                       ; edx = eax
        shr     edx, 16                        ; return dx:ax
        pop	bp
        ret

_inpd   endp

_TEXT   ends
        end
