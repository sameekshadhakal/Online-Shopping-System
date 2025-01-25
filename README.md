<html>
<head></head>
<body>
    <h1>SmartShop: An Online Shopping System</h1> 
    <h2> Project Overview </h2>
    <p>SmartShop is a comprehensive client-server application designed to provide an efficient and user-friendly online shopping experience. The system facilitates interaction between sellers and buyers, offering features such as product browsing, transaction management, feedback, and complaint handling. The project is implemented using C++ and follows a multi-threaded server-client architecture.</p>
    <h2>Features</h2>
    <h3>Client-Side Features</h3>
    <ul>
        <li><strong>Login System:</strong>
            <ul>
                <li>Sellers can log in to manage their stocks and view transactions.</li>
                <li>Buyers can sign up, log in, and shop.</li>
            </ul>
        </li>
        <li><strong>Buyer Functionalities:</strong>
            <ul>
                <li>Browse products in categories such as Food, Clothes, Stationery, Electronics, and Books.</li>
                <li>Add products to a cart and view the cart.</li>
                <li>Print a detailed bill after checkout.</li>
                <li>File complaints and view responses.</li>
            </ul>
        </li>
        <li><strong>Seller Functionalities:</strong>
            <ul>
                <li>Manage stock of items.</li>
                <li>View feedback and complaints.</li>
                <li>Check transaction history.</li>
            </ul>
        </li>
        <li><strong>Navigation:</strong>
            <ul>
                <li>Smooth navigation between menus.</li>
                <li>Options to return to the main menu or exit the program.</li>
            </ul>
        </li>
    </ul>
    <h3>Server-Side Features</h3>
    <ul>
        <li><strong>Multi-Threaded Architecture:</strong>
            <ul>
                <li>Efficiently processes client requests using a thread pool.</li>
            </ul>
        </li>
        <li><strong>Client Activity Tracking:</strong>
            <ul>
                <li>Logs connection details (IP address, port, and timestamp) of clients.</li>
                <li>Maintains activity logs in clients.txt.</li>
            </ul>
        </li>
        <li><strong>Connection Management:</strong>
            <ul>
                <li>Accepts and handles multiple connections.</li>
                <li>Gracefully closes sockets upon termination.</li>
            </ul>
        </li>
    </ul>
    <h2>File Structure</h2>
    <h3>Client Code</h3>
    <ul>
        <li><strong>sellermenu():</strong> Handles the seller interface for managing stocks, viewing feedback, and transaction history.</li>
        <li><strong>selectCategory():</strong> Allows buyers to browse and interact with products by category.</li>
        <li><strong>login():</strong> Implements the login system for both buyers and sellers.</li>
        <li><strong>main():</strong> Entry point for the client application, initializes credentials and starts the login process.</li>
    </ul>
    <h3>Server Code</h3>
    <ul>
        <li><strong>start():</strong> Initializes the server and creates worker threads.</li>
        <li><strong>Listen():</strong> Sets up the server to listen for incoming client connections.</li>
        <li><strong>processReq():</strong> Processes incoming client requests and delegates tasks to available threads.</li>
        <li><strong>threadPool():</strong> Manages thread allocation for handling client requests.</li>
        <li><strong>updateActivity():</strong> Logs client connection details to clients.txt.</li>
        <li><strong>getActivity():</strong> Reads and returns activity logs.</li>
    </ul>
    <h2>How to Run the Project</h2>
    <h3>Prerequisites</h3>
    <ul>
        <li>Windows Operating System (with Winsock library support).</li>
        <li>A C++ compiler (e.g., GCC, Visual Studio, or Clang).</li>
        <li>Basic understanding of socket programming.</li>
    </ul>
    <h3>Compilation and Execution</h3>
    <p><strong>Compile the Client Code:</strong></p>
    <pre>g++ client.cpp -o client</pre>
    <p><strong>Compile the Server Code:</strong></p>
    <pre>g++ server.cpp -o server -lws2_32</pre>
    <p><strong>Run the Server:</strong></p>
    <pre>./server</pre>
    <p><strong>Run the Client:</strong></p>
    <pre>./client</pre>
    <p><strong>Note:</strong> Ensure the server is running before starting the client application.</p>
    <h3>Configuration</h3>
    <ul>
        <li><strong>IP Address:</strong> Update the server's IP address in the client code if it is hosted remotely.</li>
        <li><strong>Port Number:</strong> Modify the port number in both client and server code as needed.</li>
    </ul>
    <h3>Logs and Tracking</h3>
    <ul>
        <li><strong>Client Logs:</strong> Connection details are stored in clients.txt.</li>
    </ul>
    <h2>Other Contributors</h2>
    <ul>
        <li>Agrima Shahi</li>
        <li>Kripa Timalsena</li>
        <li>Shristi Mallik</li>
    </ul>
    <h2>License</h2>
    <p>This project is licensed under the MIT License. Feel free to use and modify it as per your requirements.</p>
    <h2>Contact</h2>
    <p>For any questions or feedback, please contact: <a href="mailto:d.samiksha13@gmail.com">d.samiksha13@gmail.com</a></p>
</body>
</html>
