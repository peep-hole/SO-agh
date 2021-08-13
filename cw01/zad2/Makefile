CC = gcc -Wall -std=c11 -ggdb

all: test

static:
	make clean1
	$(CC) -c mylib.c
	ar crs libmylib.a mylib.o
	$(CC) main.c -o main -L. -lmylib

shared:
	make clean2
	$(CC) -fPIC -c mylib.c
	$(CC) -shared -fPIC -o libmylib.so mylib.o
	$(CC) main.c -o main -L. -lmylib -Wl,-rpath=`pwd`

test:
	make static
	make run1
	make shared	
	make run2
	make clean

clean1:
	rm -f ./raport/raport2.txt
	make clean
	rm -f *.txt

clean2:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main
	rm -f tmp.txt

clean3:
	make clean

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main
	rm -f *.txt

run1:
	@echo ***********[__ZAD2__]*****************
	python3 py_gen.py
	@echo "||||||||||||||STATIC||||||||||||||" >> ./raport/raport2.txt
	make run

run2:
	@echo "||||||||||||||SHARED||||||||||||||" >> ./raport/raport2.txt
	make run

run:
	@echo "______small_merge_______" >> ./raport/raport2.txt

	@echo ***********[__small__]*****************
	./main 250 create_table 250 merge_files 250 i0.txt:i1.txt i2.txt:i3.txt i4.txt:i5.txt i6.txt:i7.txt i8.txt:i9.txt i10.txt:i11.txt i12.txt:i13.txt i14.txt:i15.txt i16.txt:i17.txt i18.txt:i19.txt i20.txt:i21.txt i22.txt:i23.txt i24.txt:i25.txt i26.txt:i27.txt i28.txt:i29.txt i30.txt:i31.txt i32.txt:i33.txt i34.txt:i35.txt i36.txt:i37.txt i38.txt:i39.txt i40.txt:i41.txt i42.txt:i43.txt i44.txt:i45.txt i46.txt:i47.txt i48.txt:i49.txt i50.txt:i51.txt i52.txt:i53.txt i54.txt:i55.txt i56.txt:i57.txt i58.txt:i59.txt i60.txt:i61.txt i62.txt:i63.txt i64.txt:i65.txt i66.txt:i67.txt i68.txt:i69.txt i70.txt:i71.txt i72.txt:i73.txt i74.txt:i75.txt i76.txt:i77.txt i78.txt:i79.txt i80.txt:i81.txt i82.txt:i83.txt i84.txt:i85.txt i86.txt:i87.txt i88.txt:i89.txt i90.txt:i91.txt i92.txt:i93.txt i94.txt:i95.txt i96.txt:i97.txt i98.txt:i99.txt i100.txt:i101.txt i102.txt:i103.txt i104.txt:i105.txt i106.txt:i107.txt i108.txt:i109.txt i110.txt:i111.txt i112.txt:i113.txt i114.txt:i115.txt i116.txt:i117.txt i118.txt:i119.txt i120.txt:i121.txt i122.txt:i123.txt i124.txt:i125.txt i126.txt:i127.txt i128.txt:i129.txt i130.txt:i131.txt i132.txt:i133.txt i134.txt:i135.txt i136.txt:i137.txt i138.txt:i139.txt i140.txt:i141.txt i142.txt:i143.txt i144.txt:i145.txt i146.txt:i147.txt i148.txt:i149.txt i150.txt:i151.txt i152.txt:i153.txt i154.txt:i155.txt i156.txt:i157.txt i158.txt:i159.txt i160.txt:i161.txt i162.txt:i163.txt i164.txt:i165.txt i166.txt:i167.txt i168.txt:i169.txt i170.txt:i171.txt i172.txt:i173.txt i174.txt:i175.txt i176.txt:i177.txt i178.txt:i179.txt i180.txt:i181.txt i182.txt:i183.txt i184.txt:i185.txt i186.txt:i187.txt i188.txt:i189.txt i190.txt:i191.txt i192.txt:i193.txt i194.txt:i195.txt i196.txt:i197.txt i198.txt:i199.txt i200.txt:i201.txt i202.txt:i203.txt i204.txt:i205.txt i206.txt:i207.txt i208.txt:i209.txt i210.txt:i211.txt i212.txt:i213.txt i214.txt:i215.txt i216.txt:i217.txt i218.txt:i219.txt i220.txt:i221.txt i222.txt:i223.txt i224.txt:i225.txt i226.txt:i227.txt i228.txt:i229.txt i230.txt:i231.txt i232.txt:i233.txt i234.txt:i235.txt i236.txt:i237.txt i238.txt:i239.txt i240.txt:i241.txt i242.txt:i243.txt i244.txt:i245.txt i246.txt:i247.txt i248.txt:i249.txt i250.txt:i251.txt i252.txt:i253.txt i254.txt:i255.txt i256.txt:i257.txt i258.txt:i259.txt i260.txt:i261.txt i262.txt:i263.txt i264.txt:i265.txt i266.txt:i267.txt i268.txt:i269.txt i270.txt:i271.txt i272.txt:i273.txt i274.txt:i275.txt i276.txt:i277.txt i278.txt:i279.txt i280.txt:i281.txt i282.txt:i283.txt i284.txt:i285.txt i286.txt:i287.txt i288.txt:i289.txt i290.txt:i291.txt i292.txt:i293.txt i294.txt:i295.txt i296.txt:i297.txt i298.txt:i299.txt i300.txt:i301.txt i302.txt:i303.txt i304.txt:i305.txt i306.txt:i307.txt i308.txt:i309.txt i310.txt:i311.txt i312.txt:i313.txt i314.txt:i315.txt i316.txt:i317.txt i318.txt:i319.txt i320.txt:i321.txt i322.txt:i323.txt i324.txt:i325.txt i326.txt:i327.txt i328.txt:i329.txt i330.txt:i331.txt i332.txt:i333.txt i334.txt:i335.txt i336.txt:i337.txt i338.txt:i339.txt i340.txt:i341.txt i342.txt:i343.txt i344.txt:i345.txt i346.txt:i347.txt i348.txt:i349.txt i350.txt:i351.txt i352.txt:i353.txt i354.txt:i355.txt i356.txt:i357.txt i358.txt:i359.txt i360.txt:i361.txt i362.txt:i363.txt i364.txt:i365.txt i366.txt:i367.txt i368.txt:i369.txt i370.txt:i371.txt i372.txt:i373.txt i374.txt:i375.txt i376.txt:i377.txt i378.txt:i379.txt i380.txt:i381.txt i382.txt:i383.txt i384.txt:i385.txt i386.txt:i387.txt i388.txt:i389.txt i390.txt:i391.txt i392.txt:i393.txt i394.txt:i395.txt i396.txt:i397.txt i398.txt:i399.txt i400.txt:i401.txt i402.txt:i403.txt i404.txt:i405.txt i406.txt:i407.txt i408.txt:i409.txt i410.txt:i411.txt i412.txt:i413.txt i414.txt:i415.txt i416.txt:i417.txt i418.txt:i419.txt i420.txt:i421.txt i422.txt:i423.txt i424.txt:i425.txt i426.txt:i427.txt i428.txt:i429.txt i430.txt:i431.txt i432.txt:i433.txt i434.txt:i435.txt i436.txt:i437.txt i438.txt:i439.txt i440.txt:i441.txt i442.txt:i443.txt i444.txt:i445.txt i446.txt:i447.txt i448.txt:i449.txt i450.txt:i451.txt i452.txt:i453.txt i454.txt:i455.txt i456.txt:i457.txt i458.txt:i459.txt i460.txt:i461.txt i462.txt:i463.txt i464.txt:i465.txt i466.txt:i467.txt i468.txt:i469.txt i470.txt:i471.txt i472.txt:i473.txt i474.txt:i475.txt i476.txt:i477.txt i478.txt:i479.txt i480.txt:i481.txt i482.txt:i483.txt i484.txt:i485.txt i486.txt:i487.txt i488.txt:i489.txt i490.txt:i491.txt i492.txt:i493.txt i494.txt:i495.txt i496.txt:i497.txt i498.txt:i499.txt

	@echo "______medium_merge_______" >> ./raport/raport2.txt

	@echo ***********[__medium__]*****************
	./main 25 create_table 25 merge_files 25 h0.txt:h1.txt h2.txt:h3.txt h4.txt:h5.txt h6.txt:h7.txt h8.txt:h9.txt h10.txt:h11.txt h12.txt:h13.txt h14.txt:h15.txt h16.txt:h17.txt h18.txt:h19.txt h20.txt:h21.txt h22.txt:h23.txt h24.txt:h25.txt h26.txt:h27.txt h28.txt:h29.txt h30.txt:h31.txt h32.txt:h33.txt h34.txt:h35.txt h36.txt:h37.txt h38.txt:h39.txt h40.txt:h41.txt h42.txt:h43.txt h44.txt:h45.txt h46.txt:h47.txt h48.txt:h49.txt

	@echo "______large_merge_______" >> ./raport/raport2.txt

	@echo ***********[__large__]*****************
	./main 3 create_table 3 merge_files 3 g0.txt:g1.txt g2.txt:g3.txt g4.txt:g5.txt

	@echo "______merge_remove_merge_remove_(...)_______" >> ./raport/raport2.txt

	@echo ***********[__Adding and removing blocks__]*****************
	./main 251 create_table 1 merge_files 1 h1.txt:h2.txt remove_row 0 2 remove_block 0 create_table 250 merge_files 50 i0.txt:i1.txt i2.txt:i3.txt i4.txt:i5.txt i6.txt:i7.txt i8.txt:i9.txt i10.txt:i11.txt i12.txt:i13.txt i14.txt:i15.txt i16.txt:i17.txt i18.txt:i19.txt i20.txt:i21.txt i22.txt:i23.txt i24.txt:i25.txt i26.txt:i27.txt i28.txt:i29.txt i30.txt:i31.txt i32.txt:i33.txt i34.txt:i35.txt i36.txt:i37.txt i38.txt:i39.txt i40.txt:i41.txt i42.txt:i43.txt i44.txt:i45.txt i46.txt:i47.txt i48.txt:i49.txt i50.txt:i51.txt i52.txt:i53.txt i54.txt:i55.txt i56.txt:i57.txt i58.txt:i59.txt i60.txt:i61.txt i62.txt:i63.txt i64.txt:i65.txt i66.txt:i67.txt i68.txt:i69.txt i70.txt:i71.txt i72.txt:i73.txt i74.txt:i75.txt i76.txt:i77.txt i78.txt:i79.txt i80.txt:i81.txt i82.txt:i83.txt i84.txt:i85.txt i86.txt:i87.txt i88.txt:i89.txt i90.txt:i91.txt i92.txt:i93.txt i94.txt:i95.txt i96.txt:i97.txt i98.txt:i99.txt remove_row 23 43 remove_block 32 merge_files 100 i100.txt:i101.txt i102.txt:i103.txt i104.txt:i105.txt i106.txt:i107.txt i108.txt:i109.txt i110.txt:i111.txt i112.txt:i113.txt i114.txt:i115.txt i116.txt:i117.txt i118.txt:i119.txt i120.txt:i121.txt i122.txt:i123.txt i124.txt:i125.txt i126.txt:i127.txt i128.txt:i129.txt i130.txt:i131.txt i132.txt:i133.txt i134.txt:i135.txt i136.txt:i137.txt i138.txt:i139.txt i140.txt:i141.txt i142.txt:i143.txt i144.txt:i145.txt i146.txt:i147.txt i148.txt:i149.txt i150.txt:i151.txt i152.txt:i153.txt i154.txt:i155.txt i156.txt:i157.txt i158.txt:i159.txt i160.txt:i161.txt i162.txt:i163.txt i164.txt:i165.txt i166.txt:i167.txt i168.txt:i169.txt i170.txt:i171.txt i172.txt:i173.txt i174.txt:i175.txt i176.txt:i177.txt i178.txt:i179.txt i180.txt:i181.txt i182.txt:i183.txt i184.txt:i185.txt i186.txt:i187.txt i188.txt:i189.txt i190.txt:i191.txt i192.txt:i193.txt i194.txt:i195.txt i196.txt:i197.txt i198.txt:i199.txt i200.txt:i201.txt i202.txt:i203.txt i204.txt:i205.txt i206.txt:i207.txt i208.txt:i209.txt i210.txt:i211.txt i212.txt:i213.txt i214.txt:i215.txt i216.txt:i217.txt i218.txt:i219.txt i220.txt:i221.txt i222.txt:i223.txt i224.txt:i225.txt i226.txt:i227.txt i228.txt:i229.txt i230.txt:i231.txt i232.txt:i233.txt i234.txt:i235.txt i236.txt:i237.txt i238.txt:i239.txt i240.txt:i241.txt i242.txt:i243.txt i244.txt:i245.txt i246.txt:i247.txt i248.txt:i249.txt i250.txt:i251.txt i252.txt:i253.txt i254.txt:i255.txt i256.txt:i257.txt i258.txt:i259.txt i260.txt:i261.txt i262.txt:i263.txt i264.txt:i265.txt i266.txt:i267.txt i268.txt:i269.txt i270.txt:i271.txt i272.txt:i273.txt i274.txt:i275.txt i276.txt:i277.txt i278.txt:i279.txt i280.txt:i281.txt i282.txt:i283.txt i284.txt:i285.txt i286.txt:i287.txt i288.txt:i289.txt i290.txt:i291.txt i292.txt:i293.txt i294.txt:i295.txt i296.txt:i297.txt i298.txt:i299.txt remove_row 121 23 remove_block 99 merge_files 100 i300.txt:i301.txt i302.txt:i303.txt i304.txt:i305.txt i306.txt:i307.txt i308.txt:i309.txt i310.txt:i311.txt i312.txt:i313.txt i314.txt:i315.txt i316.txt:i317.txt i318.txt:i319.txt i320.txt:i321.txt i322.txt:i323.txt i324.txt:i325.txt i326.txt:i327.txt i328.txt:i329.txt i330.txt:i331.txt i332.txt:i333.txt i334.txt:i335.txt i336.txt:i337.txt i338.txt:i339.txt i340.txt:i341.txt i342.txt:i343.txt i344.txt:i345.txt i346.txt:i347.txt i348.txt:i349.txt i350.txt:i351.txt i352.txt:i353.txt i354.txt:i355.txt i356.txt:i357.txt i358.txt:i359.txt i360.txt:i361.txt i362.txt:i363.txt i364.txt:i365.txt i366.txt:i367.txt i368.txt:i369.txt i370.txt:i371.txt i372.txt:i373.txt i374.txt:i375.txt i376.txt:i377.txt i378.txt:i379.txt i380.txt:i381.txt i382.txt:i383.txt i384.txt:i385.txt i386.txt:i387.txt i388.txt:i389.txt i390.txt:i391.txt i392.txt:i393.txt i394.txt:i395.txt i396.txt:i397.txt i398.txt:i399.txt i400.txt:i401.txt i402.txt:i403.txt i404.txt:i405.txt i406.txt:i407.txt i408.txt:i409.txt i410.txt:i411.txt i412.txt:i413.txt i414.txt:i415.txt i416.txt:i417.txt i418.txt:i419.txt i420.txt:i421.txt i422.txt:i423.txt i424.txt:i425.txt i426.txt:i427.txt i428.txt:i429.txt i430.txt:i431.txt i432.txt:i433.txt i434.txt:i435.txt i436.txt:i437.txt i438.txt:i439.txt i440.txt:i441.txt i442.txt:i443.txt i444.txt:i445.txt i446.txt:i447.txt i448.txt:i449.txt i450.txt:i451.txt i452.txt:i453.txt i454.txt:i455.txt i456.txt:i457.txt i458.txt:i459.txt i460.txt:i461.txt i462.txt:i463.txt i464.txt:i465.txt i466.txt:i467.txt i468.txt:i469.txt i470.txt:i471.txt i472.txt:i473.txt i474.txt:i475.txt i476.txt:i477.txt i478.txt:i479.txt i480.txt:i481.txt i482.txt:i483.txt i484.txt:i485.txt i486.txt:i487.txt i488.txt:i489.txt i490.txt:i491.txt i492.txt:i493.txt i494.txt:i495.txt i496.txt:i497.txt i498.txt:i499.txt

