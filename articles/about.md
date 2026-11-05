# about me

Hi! My name is asklipios and I am a recreational programmer staright from Greece.
This is my little corner of the internet to post my little computer stuff and other
ponderings and shenanigans. In case you care:
- this was written in emacs
- I dualboot nixOS/ freebsd on the pc and fedora on the laptop
- in the proccess of learning haskell

# about this site
I first made the ssg for this site in lua but filesystem operations were too limited
and the markdown parser I wrote relied to much on regex. The current version is written
in C using the great [orc/discount](https://www.pell.portland.or.us/~orc/Code/discount/)
lib with my own simple template engine. I will probably write a little blog about the ssg in
the future.

```c
(void) website;
```