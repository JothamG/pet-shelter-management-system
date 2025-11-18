all: shelter

shelter: main.c
  gcc main.c -o shelter -Wall - Wextra

clean:
  rm shelter
