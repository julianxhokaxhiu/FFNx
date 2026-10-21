.386
.model flat
OPTION DOTNAME
.text$aaa SEGMENT PARA PUBLIC 'CODE'
PUBLIC _retail_image_space
_retail_image_space LABEL BYTE
DB 02C00000h DUP (0)
.text$aaa ENDS
END
