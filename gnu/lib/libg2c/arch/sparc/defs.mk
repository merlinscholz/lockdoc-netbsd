# This file is automatically generated.  DO NOT EDIT!
# Generated from: 	NetBSD: toolchain2netbsd,v 1.7 2001/08/06 23:26:28 tv Exp 
#
G_F2CEXT=abort derf derfc ef1asc ef1cmc erf erfc exit getarg getenv iargc  signal system flush ftell fseek access besj0 besj1 besjn besy0 besy1  besyn chdir chmod ctime dbesj0 dbesj1 dbesjn dbesy0 dbesy1 dbesyn  dtime etime fdate fgetc fget flush1 fnum fputc fput fstat gerror  getcwd getgid getlog getpid getuid gmtime hostnm idate ierrno irand  isatty itime kill link lnblnk lstat ltime mclock perror rand rename  secnds second sleep srand stat symlnk time ttynam umask unlink  vxttim alarm  date_y2kbuggy date_y2kbug vxtidt_y2kbuggy vxtidt_y2kbug
G_ALL_CFLAGS=-I. -I${DIST}/libf2c/libF77 -I.. -I${DIST}/libf2c/libF77/..  -DSTDC_HEADERS=1 -DRETSIGTYPE=void -DIEEE_drem=1 -DSkip_f2c_Undefs=1 -g -O2
G_OBJS=F77_aloc.o VersionF.o main.o s_rnge.o abort_.o getarg_.o iargc_.o getenv_.o signal_.o s_stop.o s_paus.o system_.o cabs.o derf_.o derfc_.o erf_.o erfc_.o sig_die.o exit_.o setarg.o setsig.o pow_ci.o pow_dd.o pow_di.o pow_hh.o pow_ii.o  pow_ri.o pow_zi.o pow_zz.o  pow_qq.o c_abs.o c_cos.o c_div.o c_exp.o c_log.o c_sin.o c_sqrt.o z_abs.o z_cos.o z_div.o z_exp.o z_log.o z_sin.o z_sqrt.o r_abs.o r_acos.o r_asin.o r_atan.o r_atn2.o r_cnjg.o r_cos.o r_cosh.o r_dim.o r_exp.o r_imag.o r_int.o r_lg10.o r_log.o r_mod.o r_nint.o r_sign.o r_sin.o r_sinh.o r_sqrt.o r_tan.o r_tanh.o d_abs.o d_acos.o d_asin.o d_atan.o d_atn2.o d_cnjg.o d_cos.o d_cosh.o d_dim.o d_exp.o d_imag.o d_int.o d_lg10.o d_log.o d_mod.o d_nint.o d_prod.o d_sign.o d_sin.o d_sinh.o d_sqrt.o d_tan.o d_tanh.o i_abs.o i_dim.o i_dnnt.o i_indx.o i_len.o i_mod.o i_nint.o i_sign.o  h_abs.o h_dim.o h_dnnt.o h_indx.o h_len.o h_mod.o  h_nint.o h_sign.o l_ge.o l_gt.o l_le.o l_lt.o hl_ge.o hl_gt.o hl_le.o hl_lt.o ef1asc_.o ef1cmc_.o s_cat.o s_cmp.o s_copy.o lbitbits.o lbitshft.o qbitbits.o qbitshft.o
G_ALL_CFLAGS+=-I. -I${DIST}/libf2c/libI77 -I.. -I${DIST}/libf2c/libI77/..   -DSTDC_HEADERS=1 -D_POSIX_SOURCE=1 -DHAVE_TEMPNAM=1 -DNO_EOF_CHAR_CHECK=1 -DSkip_f2c_Undefs=1 -g -O2
G_OBJ+=VersionI.o backspace.o close.o dfe.o dolio.o due.o endfile.o err.o  fmt.o fmtlib.o iio.o ilnw.o inquire.o lread.o lwrite.o open.o  rdfmt.o rewind.o rsfe.o rsli.o rsne.o sfe.o sue.o typesize.o uio.o  util.o wref.o wrtfmt.o wsfe.o wsle.o wsne.o xwsne.o  ftell_.o
G_ALL_CFLAGS+=-I. -I${DIST}/libf2c/libU77 -I${DIST}/libf2c/libU77/../libI77 -I..  -I${DIST}/libf2c/libU77/..  -DHAVE_CONFIG_H -g -O2
G_OBJS+=VersionU.o gerror_.o perror_.o ierrno_.o itime_.o time_.o  unlink_.o fnum_.o getpid_.o getuid_.o getgid_.o kill_.o rand_.o  srand_.o irand_.o sleep_.o idate_.o ctime_.o etime_.o  dtime_.o  isatty_.o ltime_.o fstat_.o stat_.o  lstat_.o access_.o link_.o getlog_.o ttynam_.o getcwd_.o symlnk_.o  vxttime_.o vxtidate_.o gmtime_.o fdate_.o secnds_.o  bes.o dbes.o  chdir_.o chmod_.o lnblnk_.o hostnm_.o rename_.o fgetc_.o fputc_.o  umask_.o sys_clock_.o date_.o second_.o flush1_.o mclock_.o  alarm_.o datetime_.o
