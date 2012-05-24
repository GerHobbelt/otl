
for /r %%f in ( *.html *.htm ) do tidy -ashtml -wrap 800 -i -m "%%f"

