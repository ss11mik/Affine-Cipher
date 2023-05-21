mkdir in out
cd in
split -l 1 ../ciphertext.csv

cd ..


for i in `ls in/ -1`; do
    ./kry -c -f in/$i -o out/$i

    head -c 60 out/$i
    echo ""
done
