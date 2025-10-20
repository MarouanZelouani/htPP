#include "CGIHandler.hpp"
#include <sys/wait.h>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <sched.h>
#include <vector>

CGIHandler::CGIHandler(HttpRequest const& req, LocationConfig* location, std::string const & filepath, const ServerConfig* ServerConfig)
  : 
    _location(location),
    _request(req),
    _script_filename(filepath),
    _body(req.getBody()),
    servConfig(ServerConfig)
{}

std::string CGIHandler::getInterpreter(std::string const & extension)
{
  return _location->cgi[extension];
}

std::string convertHeaderName(const std::string& headerName) {
  if (headerName.empty()) return std::string("");
  std::string new_headerName;
  new_headerName.reserve(headerName.length() + 5);
  new_headerName.replace(0, 5, "HTTP_");
  for (size_t i = 0; i < 0; ++i) {
    if (isalpha(headerName[i]) && !isupper(headerName[i]))
      new_headerName[i + 5] = std::toupper(headerName[i]);
    else if (headerName[i] == '-')
      new_headerName[i + 5] = '_';
    else 
      new_headerName[i + 5] = headerName[i];
  }
}

std::vector<std::string> CGIHandler::prepareEnv() {
    std::vector<std::string> env;
    env.push_back("GATEWAY_INTERFACE=CGI/1.0");
    env.push_back("SERVER_PROTOCOL=HTTP/1.0");
    env.push_back("SCRIPT_NAME=" + _script_filename);
    env.push_back("REQUEST_METHOD=" + _request.getMethod());
    env.push_back("QUERY_STRING=" + _request.getQueryString());
    env.push_back("SERVER_NAME=" + servConfig->server_name);
    env.push_back("SERVER_PORT=" + std::to_string(servConfig->port));
    env.push_back("SERVER_PROTOCOL=" + _request.getVersion());
    env.push_back("REMOTE_ADDR=");        // clinet ip address
    if (_request.getMethod() == "POST" && _request.getContentlength() >= 0) {
      env.push_back("CONTENT_TYPE=" + _request.getHeader("content-type"));
      env.push_back("CONTENT_LENGTH=" + std::to_string(_request.getContentlength()));
    }
    if (!_request.hasHeaders()) {
      std::map<std::string, std::string> request_headers = _request.getHeadersMap();
      std::map<std::string, std::string>::const_iterator it;
      for (it = request_headers.begin(); it != request_headers.end(); ++it) {
        if (it->first != "content-type" && it->first != "content-length")
          env.push_back(convertHeaderName(it->first) + it->second);      
      }
    }
    return env;
}

char** CGIHandler::vectorToCharArray(const std::vector<std::string>& vec) {
    char** arr = new char*[vec.size() + 1];
    for (size_t i = 0; i < vec.size(); ++i) {
        arr[i] = new char[vec[i].length() + 1];
        std::strcpy(arr[i], vec[i].c_str());
    }
    arr[vec.size()] = NULL;
    return arr;
}

void CGIHandler::freeCharArray(char** arr) {
    for (int i = 0; arr[i] != NULL; ++i) {
        delete[] arr[i];
    }
    delete[] arr;
}

// std::string CGIHandler::execute(std::string const & extension)
// {
//   int pipeIn[2];
//   int pipeOut[2];
//   if (pipe(pipeIn) == -1 || pipe(pipeOut) == -1) {
//     return "HTTP/1.1 500 Internal Server Error\r\n\r\nPipe creation failed";
//   }

//   // set pipes to non-blocking mode 

//   pid_t pid = fork();
//   if (pid == -1)
//   {
//     close(pipeIn[0]);
//     close(pipeIn[1]);
//     close(pipeOut[0]);
//     close(pipeOut[1]);
//     return "HTTP/1.1 500 Internal Server Error\r\n\r\nFork failed";
//   }
//   if (pid == 0)
//   {
//     close(pipeIn[1]);
//     close(pipeOut[0]);
//     dup2(pipeIn[0], STDIN_FILENO);
//     dup2(pipeOut[1], STDOUT_FILENO);
//     close(pipeIn[0]);
//     close(pipeOut[1]);
//     std::vector<std::string> vec = prepareEnv();
//     char **envp = vectorToCharArray(vec);
//     std::string interpreter = getInterpreter(extension);
//     if (interpreter.empty())
//     {
//       char* argv[] = { const_cast<char*>(_script_filename.c_str()), NULL };
//       execve(_script_filename.c_str(), argv, envp);
//       exit(1);
//     }
//     char* argv[] = { 
//       const_cast<char*>(interpreter.c_str()), 
//       const_cast<char*>(_script_filename.c_str()), 
//       NULL 
//     };
//     execve(interpreter.c_str(), argv, envp);
//     exit(1);
//   }
//   close(pipeIn[0]);
//   close(pipeOut[1]);
//   if (_request.getMethod() == "POST" && !_body.empty())
//     write(pipeIn[1], _body.c_str(), _body.length());
//   close(pipeIn[1]);
//   std::string output;
//   char buffer[4096];
//   ssize_t bytes_read;
//   while ((bytes_read = read(pipeOut[0], buffer, sizeof(buffer))) > 0)
//     output.append(buffer, bytes_read);
//   close(pipeOut[0]);
//   int status;
//   waitpid(pid, &status, 0);
//   if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
//     return output;
//     // if (output.find("Content-Type:") != std::string::npos ||
//     //     output.find("Content-type:") != std::string::npos) {
//     //   return "HTTP/1.0 200 OK\r\n" + output;
//     // } else {
//     //   return "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n" + output;
//     // }
//   }

//   return "HTTP/1.0 500 Internal Server Error\r\n\r\nCGI execution failed";
// }

// cleanup function to close the pipes

// std::string CGIHandler::execute(const std::string& extention) {
//   // validate script exists and executable
//   int pipeIn[2];    // for writing to CGI
//   int pipeOut[2];   //  for reading from CGI
//   pid_t pid;

//   if (pipe(pipeIn) == -1 || pipe(pipeOut) == -1)
//     return "HTTP/1.0 500 Internal Server Error\r\n\r\npipe failed";
  
//   int flags;
//   flags = fcntl(pipeOut[0], F_GETFL, 0);
//   if (flags == -1 || fcntl(pipeOut[0], F_SETFL, flags | O_NONBLOCK) == -1) {
//     close(pipeIn[0]); close(pipeIn[1]);
//     close(pipeOut[0]); close(pipeOut[1]);
//     return "HTTP/1.0 500 Internal Server Error\r\n\r\nfcntl failed";
//   }

//   flags = fcntl(pipeIn[1], F_GETFL, 0);
//   if (flags == -1 || fcntl(pipeIn[1], F_SETFL, flags | O_NONBLOCK) == -1) {
//     close(pipeIn[0]); close(pipeIn[1]);
//     close(pipeOut[0]); close(pipeOut[1]);
//     return "HTTP/1.0 500 Internal Server Error\r\n\r\nfcntl failed";
//   }

//   pid = fork();
//   if (pid == -1) {
//     close(pipeIn[0]); close(pipeIn[1]);
//     close(pipeOut[0]); close(pipeOut[1]);
//     return "HTTP/1.0 500 Internal Server Error\r\n\r\nfork failed";
//   }
//   if (pid == 0) {
//     // CHILD PROCESS
//     close(pipeIn[1]);
//     close(pipeOut[0]);

//     dup2(pipeIn[0], STDIN_FILENO);
//     close(pipeIn[0]);

//     dup2(pipeOut[1], STDOUT_FILENO);
//     close(pipeOut[1]);

//     std::vector<std::string> env_vect = prepareEnv();
//     char **envp = vectorToCharArray(env_vect);
//     std::string interpreter = getInterpreter(extention);
//     if (interpreter.empty()) {
//       char* argv[] = { const_cast<char*>(_script_filename.c_str()), NULL };
//       execve(_script_filename.c_str(), argv, envp);
//     } else {
//       char* argv[] = {
//         const_cast<char*>(interpreter.c_str()),
//         const_cast<char*>(_script_filename.c_str()),
//         NULL
//       };
//       execve(interpreter.c_str(), argv, envp);
//     }
//     freeCharArray(envp);
//     exit(1);
//   }
//   // PARENT PROCESS
//   close(pipeIn[0]);
//   close(pipeOut[1]);
//   size_t bytes_written = 0;
//   size_t total_to_write = _body.length();
//   bool stdin_closed;

//   std::string output;
//   output.reserve(BODY_CHUNK_SIZE);
  
//   bool cgi_running = true;
  
//   // set timeout for CGI execution (30sec)
//   time_t start_time = time(0);
//   time_t timeout_seconds = 30;

//   while (cgi_running) {
//     // check is CGI process exited
//     int status;
//     pid_t result = waitpid(pid, &status, WNOHANG);
//     if (result > 0) {
//       cgi_running = false;
//       break;
//     } else {
//       close(pipeOut[0]); close(pipeIn[1]);
//       return "HTTP/1.0 500 Internal Server Error\r\n\r\nWaitpid failed";
//     }

//     // check for timeout
//     time_t elapsed = time(0) - start_time;
//     if (elapsed >= timeout_seconds) {
//       // TIMEOUT!!
//       kill(pid, SIGKILL);
//       waitpid(pid, NULL, 0);
//       close(pipeIn[1]);
//       close(pipeOut[0]);
//       return "HTTP/1.0 500 Internal Server Error\r\n\r\nCGI timeout";
//     }

//     // calculatin remaining time for poll
//     time_t remaining = timeout_seconds - elapsed;
//     int timeout_ms = remaining * 1000;

//     struct pollfd fds[2];
//     int nfds = 0;

//     // monitor pipeOut for reading CGI output
//     fds[nfds].fd = pipeOut[0];
//     fds[nfds].events = POLLIN;
//     fds[nfds].revents = 0;
//     int pipeout_index = nfds;
//     nfds++;

//     // monitor pipein for writing POST data
//     int pipein_index = -1;
//     if (!stdin_closed && bytes_written < total_to_write) {
//       fds[nfds].fd = pipeIn[1];
//       fds[nfds].events = POLLOUT;
//       fds[nfds].revents = 0;
//       pipein_index = nfds;
//       nfds++;
//     }

//     int pollResult = poll(fds, nfds, timeout_ms);
//     if (pollResult == -1) {
//       if (errno == EINTR)
//         continue;
//       kill(pid, SIGKILL);
//       waitpid(pid, NULL, 0);
//       close(pipeIn[1]);
//       close(pipeOut[0]);
//       return "HTTP/1.0 500 Internal Server Error\r\n\r\npoll vailed";
//     }
//     if (pollResult == 0)
//       continue;

//     if (fds[pipeout_index].revents & POLLIN) {
//       // TODO: Read from CGI output (next step)
//     }
        
//     // Check for errors on pipeOut
//     if (fds[pipeout_index].revents & (POLLERR | POLLHUP)) {
//     // Pipe closed or error
//     // CGI might have finished, continue to next iteration
//     }
        
//     // Check if pipeIn is ready for writing
//     if (pipein_index != -1 && (fds[pipein_index].revents & POLLOUT)) {
//         // TODO: Write POST data to CGI (next step)
//     }

//     if (pipein_index != -1 && (fds[pipein_index].revents & (POLLERR | POLLHUP))) {
//       // Can't write anymore, close it
//       if (!stdin_closed) {
//         close(pipeIn[1]);
//         stdin_closed = true;
//       }
//     }
//   }
// }