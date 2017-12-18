OUT=./garbage/
SRC=./src/
INC=./inc/
SER=server.out
STAT=statistic.out
CL=client.out
TSTP=test.out
TEST=./tests/
RES=./results/
build:
	./scripts/buildsys.sh
runser: 
	$(OUT)/$(SER) $(OUT)/$(TSTP)
runstat: 
	$(OUT)/$(STAT) "Topic #1"
runclient: 
	$(OUT)/$(CL) "Topic #1"
test1: build
	mkdir -p $(RES)	
	$(OUT)/$(CL) <$(TEST)/test1 >$(RES)/res-1 &
	$(OUT)/$(CL) "Topic #1" <$(TEST)/test1 >$(RES)/res0 &
	$(OUT)/$(SER) >$(RES)/ser0 &
	$(OUT)/$(SER) $(OUT)/$(TSTP) <$(TEST)/stat1 >$(RES)/ser1 &
	sleep 0.1
	$(OUT)/$(STAT) "Topic #1" <$(TEST)/stat1 >$(RES)/resstat0 &
	sleep 0.1
	$(OUT)/$(CL) "Topic #1" <$(TEST)/test1 >$(RES)/res1 &
	$(OUT)/$(SER) $(OUT)/$(TSTP) >$(RES)/ser2 &
	$(OUT)/$(CL) "Topic #1" <$(TEST)/test2 >$(RES)/res2 &
	$(OUT)/$(CL) "Topic #1" <$(TEST)/test3 >$(RES)/res3 &
	$(OUT)/$(STAT) "Topic #1" <$(TEST)/stat1 >$(RES)/resstat1 &
	$(OUT)/$(STAT) "Topic #1" <$(TEST)/stat2 >$(RES)/resstat2 &
	sleep 0.1
	$(OUT)/$(CL) "Topic #1" <$(TEST)/test4 >$(RES)/res4 &
	$(OUT)/$(CL) "Topic #1" <$(TEST)/test5 >$(RES)/res5 &
	$(OUT)/$(CL) "Topic #1" <$(TEST)/test6 >$(RES)/res6 &
	$(OUT)/$(CL) "Topip 1/2" <$(TEST)/test5 >$(RES)/res7 &
test2:
	$(OUT)/$(CL) "Topic #1" <$(TEST)/test1 >$(RES)/res8 &
	sleep 5
	$(OUT)/$(SER) $(OUT)/$(TSTP) >$(RES)/ser3 &
	echo client is running
	$(OUT)/$(CL) "Topic #1" >$(RES)/res9
	$(OUT)/$(CL) "Topic #1" <$(TEST)/test1 >$(RES)/res10 &
	ps -a 
	$(OUT)/$(STAT) "Topic 1" <$(TEST)/stat1 >$(RES)/resstat3 &
	$(OUT)/$(STAT) <$(TEST)/stat1 >$(RES)/resstat4 &
	echo statistic is running
	$(OUT)/$(STAT) "Topic #1" >$(RES)/resstat5
	$(OUT)/$(CL) "Topic #1" 
	$(OUT)/$(CL) "Topic #1" <$(TEST)/test7 >$(RES)/res11 &
	sleep 0.5
	$(OUT)/$(STAT) "Topic #1"<$(TEST)/stat3 2>$(RES)/restat6 1>$(RES)/restat6 &
coverage:
	./scripts/syscoverage.sh
clear:
	./scripts/clear.sh
