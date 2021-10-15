exp.jar: exp/shell.class
	zip $@ $$(find -name '*.class')

%.class: %.java
	javac $<

clean:
	rm -f exp/*.class exp.jar
