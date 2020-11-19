# MessagePattern Demo

Sample output of demo code that parses MessageFormat pattern strings via the
MessagePattern class and pretty-prints the parsed structure. Some of the
patterns are formatted via a "mini formatter".

message: Hello!

0: MSG_START(0)@0

1: MSG_LIMIT(0)@6

message: Hel'lo!

autoQA: Hel''lo!

0: MSG_START(0)@0

1: INSERT_CHAR(39)@4

2: MSG_LIMIT(0)@7

message: Hel'{o

autoQA: Hel'{o'

0: MSG_START(0)@0

1: SKIP_SYNTAX(0)@3="'"

2: INSERT_CHAR(39)@6

3: MSG_LIMIT(0)@6

message: Hel'{'o

0: MSG_START(0)@0

1: SKIP_SYNTAX(0)@3="'"

2: SKIP_SYNTAX(0)@5="'"

3: MSG_LIMIT(0)@7

message: a'{bc''de'f

0: MSG_START(0)@0

1: SKIP_SYNTAX(0)@1="'"

2: SKIP_SYNTAX(0)@6="'"

3: SKIP_SYNTAX(0)@9="'"

4: MSG_LIMIT(0)@11

a{bc'def

message: a'{bc''de'f{0,number,g'hi''jk'l#}

0: MSG_START(0)@0

1: SKIP_SYNTAX(0)@1="'"

2: SKIP_SYNTAX(0)@6="'"

3: SKIP_SYNTAX(0)@9="'"

4: ARG_START(SIMPLE)@11="{"

5: ARG_NUMBER(0)@12="0"

6: ARG_TYPE(0)@14="number"

7: ARG_STYLE(0)@21="g'hi''jk'l#"

8: ARG_LIMIT(SIMPLE)@32="}"

9: MSG_LIMIT(0)@33

message: abc{0}def

0: MSG_START(0)@0

1: ARG_START(NONE)@3="{"

2: ARG_NUMBER(0)@4="0"

3: ARG_LIMIT(NONE)@5="}"

4: MSG_LIMIT(0)@9

message: abc{ arg }def

0: MSG_START(0)@0

1: ARG_START(NONE)@3="{"

2: ARG_NAME(0)@5="arg"

3: ARG_LIMIT(NONE)@9="}"

4: MSG_LIMIT(0)@13

message: abc{1}def{arg}ghi

0: MSG_START(0)@0

1: ARG_START(NONE)@3="{"

2: ARG_NUMBER(1)@4="1"

3: ARG_LIMIT(NONE)@5="}"

4: ARG_START(NONE)@9="{"

5: ARG_NAME(0)@10="arg"

6: ARG_LIMIT(NONE)@13="}"

7: MSG_LIMIT(0)@17

message: abc{2, number}ghi{3, select, xx {xxx} other {ooo}} xyz

0: MSG_START(0)@0

1: ARG_START(SIMPLE)@3="{"

2: ARG_NUMBER(2)@4="2"

3: ARG_TYPE(0)@7="number"

4: ARG_LIMIT(SIMPLE)@13="}"

5: ARG_START(SELECT)@17="{"

6: ARG_NUMBER(3)@18="3"

7: ARG_SELECTOR(0)@29="xx"

8: MSG_START(1)@32="{"

9: MSG_LIMIT(1)@36="}"

10: ARG_SELECTOR(0)@38="other"

11: MSG_START(1)@44="{"

12: MSG_LIMIT(1)@48="}"

13: ARG_LIMIT(SELECT)@49="}"

14: MSG_LIMIT(0)@54

message: abc{gender,select,other{His name is {person,XML,<entry
name="PERSON">{$PERSON}</entry>}.}}xyz

0: MSG_START(0)@0

1: ARG_START(SELECT)@3="{"

2: ARG_NAME(0)@4="gender"

3: ARG_SELECTOR(0)@18="other"

4: MSG_START(1)@23="{"

5: ARG_START(SIMPLE)@36="{"

6: ARG_NAME(0)@37="person"

7: ARG_TYPE(0)@44="XML"

8: ARG_STYLE(0)@48="<entry name="PERSON">{$PERSON}</entry>"

9: ARG_LIMIT(SIMPLE)@86="}"

10: MSG_LIMIT(1)@88="}"

11: ARG_LIMIT(SELECT)@89="}"

12: MSG_LIMIT(0)@93

message: abc{num_people, plural, offset:17 few{fff} other {oooo}}xyz

0: MSG_START(0)@0

1: ARG_START(PLURAL)@3="{"

2: ARG_NAME(0)@4="num_people"

3: ARG_INT(17)@31="17"=17.0

4: ARG_SELECTOR(0)@34="few"

5: MSG_START(1)@37="{"

6: MSG_LIMIT(1)@41="}"

7: ARG_SELECTOR(0)@43="other"

8: MSG_START(1)@49="{"

9: MSG_LIMIT(1)@54="}"

10: ARG_LIMIT(PLURAL)@55="}"

11: MSG_LIMIT(0)@59

message: abc{ num , plural , offset: 2 =1 {1} =-1 {-1} =3.14 {3.14} other {oo}
}xyz

0: MSG_START(0)@0

1: ARG_START(PLURAL)@3="{"

2: ARG_NAME(0)@5="num"

3: ARG_INT(2)@28="2"=2.0

4: ARG_SELECTOR(0)@30="=1"

5: ARG_INT(1)@31="1"=1.0

6: MSG_START(1)@33="{"

7: MSG_LIMIT(1)@35="}"

8: ARG_SELECTOR(0)@37="=-1"

9: ARG_INT(-1)@38="-1"=-1.0

10: MSG_START(1)@41="{"

11: MSG_LIMIT(1)@44="}"

12: ARG_SELECTOR(0)@46="=3.14"

13: ARG_DOUBLE(0)@47="3.14"=3.14

14: MSG_START(1)@52="{"

15: MSG_LIMIT(1)@57="}"

16: ARG_SELECTOR(0)@59="other"

17: MSG_START(1)@65="{"

18: MSG_LIMIT(1)@68="}"

19: ARG_LIMIT(PLURAL)@70="}"

20: MSG_LIMIT(0)@74

message: I don't {a,plural,other{w'{'on't #'#'}} and {b,select,other{shan't'}'}}
'{'''know'''}' and {c,choice,0#can't'|'}{z,number,#'#'###.00'}'}.

autoQA: I don''t {a,plural,other{w'{'on''t #'#'}} and
{b,select,other{shan''t'}'}} '{'''know'''}' and
{c,choice,0#can''t'|'}{z,number,#'#'###.00'}'}.

0: MSG_START(0)@0

1: INSERT_CHAR(39)@6

2: ARG_START(PLURAL)@8="{"

3: ARG_NAME(0)@9="a"

4: ARG_SELECTOR(0)@18="other"

5: MSG_START(1)@23="{"

6: SKIP_SYNTAX(0)@25="'"

7: SKIP_SYNTAX(0)@27="'"

8: INSERT_CHAR(39)@31

9: REPLACE_NUMBER(0)@33="#"

10: SKIP_SYNTAX(0)@34="'"

11: SKIP_SYNTAX(0)@36="'"

12: MSG_LIMIT(1)@37="}"

13: ARG_LIMIT(PLURAL)@38="}"

14: ARG_START(SELECT)@44="{"

15: ARG_NAME(0)@45="b"

16: ARG_SELECTOR(0)@54="other"

17: MSG_START(1)@59="{"

18: INSERT_CHAR(39)@65

19: SKIP_SYNTAX(0)@66="'"

20: SKIP_SYNTAX(0)@68="'"

21: MSG_LIMIT(1)@69="}"

22: ARG_LIMIT(SELECT)@70="}"

23: SKIP_SYNTAX(0)@72="'"

24: SKIP_SYNTAX(0)@75="'"

25: SKIP_SYNTAX(0)@76="'"

26: SKIP_SYNTAX(0)@82="'"

27: SKIP_SYNTAX(0)@83="'"

28: SKIP_SYNTAX(0)@85="'"

29: ARG_START(CHOICE)@91="{"

30: ARG_NAME(0)@92="c"

31: ARG_INT(0)@101="0"=0.0

32: ARG_SELECTOR(0)@102="#"

33: MSG_START(1)@103

34: INSERT_CHAR(39)@107

35: SKIP_SYNTAX(0)@108="'"

36: SKIP_SYNTAX(0)@110="'"

37: MSG_LIMIT(1)@111

38: ARG_LIMIT(CHOICE)@111="}"

39: ARG_START(SIMPLE)@112="{"

40: ARG_NAME(0)@113="z"

41: ARG_TYPE(0)@115="number"

42: ARG_STYLE(0)@122="#'#'###.00'}'"

43: ARG_LIMIT(SIMPLE)@135="}"

44: MSG_LIMIT(0)@137

message: a_{0,choice,-∞ #-inf| 5≤ five | 99 # ninety'|'nine }_z

0: MSG_START(0)@0

1: ARG_START(CHOICE)@2="{"

2: ARG_NUMBER(0)@3="0"

3: ARG_DOUBLE(0)@12="-∞"=-Infinity

4: ARG_SELECTOR(0)@15="#"

5: MSG_START(1)@16

6: MSG_LIMIT(1)@20="|"

7: ARG_INT(5)@23="5"=5.0

8: ARG_SELECTOR(0)@24="≤"

9: MSG_START(1)@25

10: MSG_LIMIT(1)@31="|"

11: ARG_INT(99)@33="99"=99.0

12: ARG_SELECTOR(0)@36="#"

13: MSG_START(1)@37

14: SKIP_SYNTAX(0)@44="'"

15: SKIP_SYNTAX(0)@46="'"

16: MSG_LIMIT(1)@53

17: ARG_LIMIT(CHOICE)@53="}"

18: MSG_LIMIT(0)@56

message: a_{0,plural,other{num=#'#'=#'#'={1,number,##}!}}_z

0: MSG_START(0)@0

1: ARG_START(PLURAL)@2="{"

2: ARG_NUMBER(0)@3="0"

3: ARG_SELECTOR(0)@12="other"

4: MSG_START(1)@17="{"

5: REPLACE_NUMBER(0)@22="#"

6: SKIP_SYNTAX(0)@23="'"

7: SKIP_SYNTAX(0)@25="'"

8: REPLACE_NUMBER(0)@27="#"

9: SKIP_SYNTAX(0)@28="'"

10: SKIP_SYNTAX(0)@30="'"

11: ARG_START(SIMPLE)@32="{"

12: ARG_NUMBER(1)@33="1"

13: ARG_TYPE(0)@35="number"

14: ARG_STYLE(0)@42="##"

15: ARG_LIMIT(SIMPLE)@44="}"

16: MSG_LIMIT(1)@46="}"

17: ARG_LIMIT(PLURAL)@47="}"

18: MSG_LIMIT(0)@50

message: }}}{0}}

0: MSG_START(0)@0

1: ARG_START(NONE)@3="{"

2: ARG_NUMBER(0)@4="0"

3: ARG_LIMIT(NONE)@5="}"

4: MSG_LIMIT(0)@7

message: Hello {0}!

0: MSG_START(0)@0

1: ARG_START(NONE)@6="{"

2: ARG_NUMBER(0)@7="0"

3: ARG_LIMIT(NONE)@8="}"

4: MSG_LIMIT(0)@10

Hello Alice!

message: ++{0, select, female{{1} calls you her friend}other{{1} calls you
'{their}' friend}male{{1} calls you his friend}}--

0: MSG_START(0)@0

1: ARG_START(SELECT)@2="{"

2: ARG_NUMBER(0)@3="0"

3: ARG_SELECTOR(0)@14="female"

4: MSG_START(1)@20="{"

5: ARG_START(NONE)@21="{"

6: ARG_NUMBER(1)@22="1"

7: ARG_LIMIT(NONE)@23="}"

8: MSG_LIMIT(1)@45="}"

9: ARG_SELECTOR(0)@46="other"

10: MSG_START(1)@51="{"

11: ARG_START(NONE)@52="{"

12: ARG_NUMBER(1)@53="1"

13: ARG_LIMIT(NONE)@54="}"

14: SKIP_SYNTAX(0)@66="'"

15: SKIP_SYNTAX(0)@74="'"

16: MSG_LIMIT(1)@82="}"

17: ARG_SELECTOR(0)@83="male"

18: MSG_START(1)@87="{"

19: ARG_START(NONE)@88="{"

20: ARG_NUMBER(1)@89="1"

21: ARG_LIMIT(NONE)@90="}"

22: MSG_LIMIT(1)@112="}"

23: ARG_LIMIT(SELECT)@113="}"

24: MSG_LIMIT(0)@116

++Alice calls you her friend--

message: ++{0, select, female{{1} calls you her friend}other{{1} calls you
'{their}' friend}male{{1} calls you his friend}}--

0: MSG_START(0)@0

1: ARG_START(SELECT)@2="{"

2: ARG_NUMBER(0)@3="0"

3: ARG_SELECTOR(0)@14="female"

4: MSG_START(1)@20="{"

5: ARG_START(NONE)@21="{"

6: ARG_NUMBER(1)@22="1"

7: ARG_LIMIT(NONE)@23="}"

8: MSG_LIMIT(1)@45="}"

9: ARG_SELECTOR(0)@46="other"

10: MSG_START(1)@51="{"

11: ARG_START(NONE)@52="{"

12: ARG_NUMBER(1)@53="1"

13: ARG_LIMIT(NONE)@54="}"

14: SKIP_SYNTAX(0)@66="'"

15: SKIP_SYNTAX(0)@74="'"

16: MSG_LIMIT(1)@82="}"

17: ARG_SELECTOR(0)@83="male"

18: MSG_START(1)@87="{"

19: ARG_START(NONE)@88="{"

20: ARG_NUMBER(1)@89="1"

21: ARG_LIMIT(NONE)@90="}"

22: MSG_LIMIT(1)@112="}"

23: ARG_LIMIT(SELECT)@113="}"

24: MSG_LIMIT(0)@116

++Bob calls you his friend--

message: ++{0, select, female{{1} calls you her friend}other{{1} calls you
'{their}' friend}male{{1} calls you his friend}}--

0: MSG_START(0)@0

1: ARG_START(SELECT)@2="{"

2: ARG_NUMBER(0)@3="0"

3: ARG_SELECTOR(0)@14="female"

4: MSG_START(1)@20="{"

5: ARG_START(NONE)@21="{"

6: ARG_NUMBER(1)@22="1"

7: ARG_LIMIT(NONE)@23="}"

8: MSG_LIMIT(1)@45="}"

9: ARG_SELECTOR(0)@46="other"

10: MSG_START(1)@51="{"

11: ARG_START(NONE)@52="{"

12: ARG_NUMBER(1)@53="1"

13: ARG_LIMIT(NONE)@54="}"

14: SKIP_SYNTAX(0)@66="'"

15: SKIP_SYNTAX(0)@74="'"

16: MSG_LIMIT(1)@82="}"

17: ARG_SELECTOR(0)@83="male"

18: MSG_START(1)@87="{"

19: ARG_START(NONE)@88="{"

20: ARG_NUMBER(1)@89="1"

21: ARG_LIMIT(NONE)@90="}"

22: MSG_LIMIT(1)@112="}"

23: ARG_LIMIT(SELECT)@113="}"

24: MSG_LIMIT(0)@116

++sushifan3 calls you {their} friend--

message: _'__{gender, select, female{Her n'ame is {person_name}.}other{His n'ame
is {person_name}.}}__'_

autoQA: _''__{gender, select, female{Her n''ame is {person_name}.}other{His
n''ame is {person_name}.}}__''_

0: MSG_START(0)@0

1: INSERT_CHAR(39)@2

2: ARG_START(SELECT)@4="{"

3: ARG_NAME(0)@5="gender"

4: ARG_SELECTOR(0)@21="female"

5: MSG_START(1)@27="{"

6: INSERT_CHAR(39)@34

7: ARG_START(NONE)@41="{"

8: ARG_NAME(0)@42="person_name"

9: ARG_LIMIT(NONE)@53="}"

10: MSG_LIMIT(1)@55="}"

11: ARG_SELECTOR(0)@56="other"

12: MSG_START(1)@61="{"

13: INSERT_CHAR(39)@68

14: ARG_START(NONE)@75="{"

15: ARG_NAME(0)@76="person_name"

16: ARG_LIMIT(NONE)@87="}"

17: MSG_LIMIT(1)@89="}"

18: ARG_LIMIT(SELECT)@90="}"

19: INSERT_CHAR(39)@94

20: MSG_LIMIT(0)@95

_'__Her n'ame is Alice.__'_

message: _'__{gender, select, female{Her n'ame is {person_name}.}other{His n'ame
is {person_name}.}}__'_

autoQA: _''__{gender, select, female{Her n''ame is {person_name}.}other{His
n''ame is {person_name}.}}__''_

0: MSG_START(0)@0

1: INSERT_CHAR(39)@2

2: ARG_START(SELECT)@4="{"

3: ARG_NAME(0)@5="gender"

4: ARG_SELECTOR(0)@21="female"

5: MSG_START(1)@27="{"

6: INSERT_CHAR(39)@34

7: ARG_START(NONE)@41="{"

8: ARG_NAME(0)@42="person_name"

9: ARG_LIMIT(NONE)@53="}"

10: MSG_LIMIT(1)@55="}"

11: ARG_SELECTOR(0)@56="other"

12: MSG_START(1)@61="{"

13: INSERT_CHAR(39)@68

14: ARG_START(NONE)@75="{"

15: ARG_NAME(0)@76="person_name"

16: ARG_LIMIT(NONE)@87="}"

17: MSG_LIMIT(1)@89="}"

18: ARG_LIMIT(SELECT)@90="}"

19: INSERT_CHAR(39)@94

20: MSG_LIMIT(0)@95

_'__His n'ame is Bob.__'_
