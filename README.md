# HTTP Parser
**Author**:Constantin Radu-Giorgian  
## Description
This project utilizes the Parson library for parsing JSON messages. The implementation includes modifications to various files and the creation of new functionalities.

## Implementation Details
- The assignment was implemented based on Lab 9, utilizing the Parson library for parsing JSON messages.
- The work time spent on the assignment was approximately 10 hours.
- Files `buffer.c`, `buffer.h`, `parson.c`, and `parson.h` are unmodified and were taken from the respective laboratory and GitHub.
- Modifications and new functionalities were implemented in files `requests.c`, `requests.h`, `helpers.c`, `helpers.h`, and `client.c`.

## Key Components
- **Requests Handling**: Modifications were made to optimize function parameters in `requests.c` and `requests.h`, and `compute_delete_request` was created to delete books.
- **Helper Functions**: Two new functions were created in `helpers.c` (along with `helpers.h`) to extract the cookie and token from the server response.
- **Client Connectivity**: Implemented connection to the server and command execution in `client.c`.

## Usage
- The project allows users to perform various actions via commands, including user registration, login, accessing the library, managing books, and logging out.
- Each command corresponds to a specific functionality and involves sending requests to the server to perform the desired action.
- After executing each command, the connection is closed, and the results or corresponding error messages are displayed.
