
.silent:

TARGET ?= c64

###############################################################################

all: generic c64 vic20 pet nes atari conio tgi geos

.PHONY: generic c64 vic20 pet nes atari conio tgi geos

generic:
	make -C entp-0.7 TARGET=$(TARGET)
	make -C fixedpoint TARGET=$(TARGET)
	make -C huffman-1.0 TARGET=$(TARGET)
	make -C mmind-1.0.0 TARGET=$(TARGET)
	make -C sudoku65 TARGET=$(TARGET)
	make -C speedcode-1.0 TARGET=$(TARGET)
	make -C sqrtv1.0 TARGET=$(TARGET)

c64:
	make -C autorun-1.5.0
# TODO: c128, c610
	make -C hwclock-1.0 TARGET=$(TARGET)
	make -C loadkoala-1.0
#	make -C picoos-demo-0.8.0
	make -C se.g.i.t.a.i.-1.0
vic20:
	make -C quikman2k8-2008.11.1
pet:
# TODO: binaries do not work?
	make -C pet_tetris

nes:
	make -C nescontrollertest-1.0
atari:
# TODO: binaries do not work?
	make -C calomin
lynx:
# TODO: does not build
#	make -C lynx-cart-demo-1.1

conio:
	make -C circle-1.0 TARGET=$(TARGET)
	make -C even_odd-1.0 TARGET=$(TARGET)
	make -C isolinev0.6 TARGET=$(TARGET)
	make -C matrixcevi TARGET=$(TARGET)
	make -C simplemenu TARGET=$(TARGET)
	make -C randfade-1.0 TARGET=$(TARGET)

tgi:
# TODO: c128-vdc, atari800, apple2
	make -C 3dmaze-0.2 TARGET=$(TARGET)
# TODO: c128-vdc, atari800, apple2
	make -C circlefracv1-1 TARGET=$(TARGET)
	make -C darth-vader TARGET=$(TARGET)
# TODO: c128-vdc, atari800, apple2
	make -C drachenv1.1 TARGET=$(TARGET)
# TODO: c128-vdc, atari800, apple2
	make -C sierpinski-1.1 TARGET=$(TARGET)
# TODO: c128-vdc, atari800, apple2
	make -C tgi-preview-0.0.1 TARGET=$(TARGET)

geos:
	make -C aclock-geos TARGET=$(TARGET)

###############################################################################

clean:
# generic
	make -C entp-0.7 clean
	make -C fixedpoint clean
	make -C huffman-1.0 clean
	make -C mmind-1.0.0 clean
	make -C sudoku65 clean
	make -C speedcode-1.0 clean
	make -C sqrtv1.0 clean
# c64
	make -C autorun-1.5.0 clean
	make -C hwclock-1.0 clean
	make -C loadkoala-1.0 clean
#	make -C picoos-demo-0.8.0 clean
	make -C se.g.i.t.a.i.-1.0 clean
# vic20
	make -C quikman2k8-2008.11.1 clean
# pet
	make -C pet_tetris clean
# nes
	make -C nescontrollertest-1.0 clean
# atari
	make -C calomin clean
# lynx
	make -C lynx-cart-demo-1.1 clean
# conio
	make -C circle-1.0 clean
	make -C even_odd-1.0 clean
	make -C matrixcevi clean
	make -C isolinev0.6 clean
	make -C matrixcevi clean
	make -C simplemenu clean
	make -C randfade-1.0 clean
# tgi
	make -C 3dmaze-0.2 clean
	make -C circlefracv1-1 clean
	make -C darth-vader clean
	make -C drachenv1.1 clean
	make -C sierpinski-1.1 clean
	make -C tgi-preview-0.0.1 clean
# geos
	make -C aclock-geos clean
