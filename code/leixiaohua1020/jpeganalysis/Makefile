
# 0m2.980s (gcc version 4.0.3 20060115)
# 0m3.113s (gcc version 3.4.5)
# 0m3.307s (gcc version 3.3.6)
#CFLAGS := -O6 -march=pentium3

# 0m3.082s (gcc version 4.0.3 20060115)
# 0m3.069s (gcc version 3.4.5)
# 0m3.294s (gcc version 3.3.6)
#CFLAGS := -O2 -march=pentium3

# 0m3.103s (gcc version 4.0.3 20060115)
# 0m2.902s (gcc version 3.4.5)
# 0m3.011s (gcc version 3.3.6)
CFLAGS := -Os -march=pentium-m

CFLAGS += -g -Wall -fno-inline-functions-called-once -Wextra 
CFLAGS += -DDEBUG=1
#CFLAGS += -DDEBUG=0

#CFLAGS += -g -fprofile-arcs -ftest-coverage -fbranch-probabilities
#LDFLAGS := -fprofile-arcs -ftest-coverage 

#CFLAGS += -g -fbranch-probabilities
#LDFLAGS := 

all: loadjpeg

loadjpeg: tinyjpeg.o loadjpeg.o jidctflt.o
	$(CC) -o $@ $(LDFLAGS) $^

tinyjpeg.o: tinyjpeg.c tinyjpeg.h tinyjpeg-internal.h

loadjpeg.o: loadjpeg.c tinyjpeg.h

clean:
	$(RM) loadjpeg *.o *.gcda *.gcno

tests: loadjpeg
	time -p $(MAKE) __tests

/tmp/testrgb-1x1.jpg:
	# Copy files in /tmp (because i'm using a tmpfs filesystem)
	cp test_images/testrgb-*.jpg /tmp

__tests: /tmp/testrgb-1x1.jpg
	for fmt in rgb24 bgr24 grey yuv420p; do \
	  for x in 1 2 ; do for y in 1 2 ; do \
	    loadjpeg /tmp/testrgb-$${x}x$$y.jpg $${fmt} /tmp/testrgb-$${x}x$$y-$${fmt}; \
	    done; done; \
	done;

check: tests
	md5sum -c testrgb.md5sums
	


