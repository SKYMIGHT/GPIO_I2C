.386p
_TEXT   segment byte public use16 'CODE'
_TEXT   ends
_BSS    segment word public use16 'BSS'
_BSS    ends


DGROUP  group   _BSS
        assume  cs:_TEXT,ds:DGROUP
_TEXT   segment byte public use16 'CODE'
        assume  cs:_TEXT, ds:DGROUP

        assume ds:DGROUP

        public _outp, _outpw, _outpd

_outp   proc

	push    bp                             ; save bp to stack
        mov     bp,sp                          ; bp = sp
        xor     eax,eax
        mov     dx,word ptr [bp + 4]
        mov     al,byte ptr [bp + 8]
        out     dx,al
        pop	bp
        ret

_outp   endp


_outpw  proc

	push    bp                             ; save bp to stack
        mov     bp,sp                          ; bp = sp
        mov     dx,word ptr [bp + 4]
        mov     ax,word ptr [bp + 8]
        out     dx,ax
        pop	bp
        ret

_outpw  endp


_outpd  proc

	push    bp                             ; save bp to stack
        mov     bp,sp                          ; bp = sp
        mov     dx,word ptr [bp + 4]
        mov     eax,[bp + 8]
        out     dx,eax
        pop	bp
        ret

_outpd  endp

_TEXT   ends
        end
