```cpp

// You can create an AST Node like this. Look inside the IntegerLiteral 
	// constructor to see what you need to put into make_unique.
	std::unique_ptr<const Node> intNode = std::make_unique<const IntegerLiteral>(100);	

	// A lot of times the constructor of your node takes an r-value reference
	//
	// 	explicit CodeBlock(StatementList &&statements);
	//								      ^
	//							           L__ this wants an r-value reference
	//
	// There you should std::move the value into the make_unique.
	
    StatementList statements;
	statements.push_back(std::make_unique<IntegerLiteral>(10));
	statements.push_back(std::make_unique<StringLiteral>("TEST"));
	statements.push_back(std::make_unique<DoubleLiteral>(10.0));
	statements.push_back(std::make_unique<VariableUse>("var"));

    std::unique_ptr<CodeBlock> block = std::make_unique<CodeBlock>(std::move(statements));


	// If you put a literal object into a constructor, then you 
	// should not move the value. A literal object already is a r-value.
	
	std::unique_ptr<const Node> strNode1 = std::make_unique<const StringLiteral>(std::string("Don't move me!"));
	//													     This is an r-value <~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    std::string text2 = "You can move me if you want!";
    std::unique_ptr<const Node> strNode2 = std::make_unique<const StringLiteral>(std::move(text2));
	//                                                       text is moved here <~~~~~~~~~~~~~~~
    // Using text here is undefined behavior, it was previously moved. If you want to use
    // value after it has been moved, you should not have moved it - you could have copied it.
    //
    // std::cout << text << std::endl;                       The following would be illegal


    std::string text3 = "You can copy me if you want!";
    std::unique_ptr<const Node> strNode3 = std::make_unique<const StringLiteral>(text3);
	//                                                      text is copied here <~~~~
    std::cout << text3 << std::endl;                     // This is fine


	// If you want to relocate a unique_ptr, you canno't copy it - you have to
	// move it. Its because only one unique_ptr can own the pointer at a time:
	std::unique_ptr<const Node> strNodeMoved = std::move(strNode1);

	// Just like the text variable before, using strNode after it has been moved 
    // is undefined behavior and should never be done. Some IDEs will warn you - some won't:
	//
	// strNode->toString(std::cout);		        - DON'T DO THIS
	
	std::cout << *strNodeMoved << std::endl;       // This is fine
```