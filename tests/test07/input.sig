PROGRAM TEST07;
    VAR
	ITEM1 : INTEGER;
	ITEM2 : INTEGER;
	ITEM3 : INTEGER;
    BEGIN		
        ITEM2 := 10 >= ITEM3; 
        ITEM3 := NOT 10 <> 10;
	ITEM3 := NOT 10 <> 10 AND ITEM2 < ITEM1;
        ITEM2 := ITEM2 <= 5 OR 12 = ITEM1;
        ITEM3 := NOT [10 <> 10 AND ITEM2 < ITEM1];(**)
    END.