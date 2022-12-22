#include "request.h"

HTTP::Status::Status(int num, std::string description) {
  m_status_num = num;
  m_description = description;
  if (100 <= num && num <= 199) {
    m_class = "Informational";
  } else if (200 <= num && num <= 299) {
    m_class = "Successful";
  } else if (300 <= num && num <= 399) {
    m_class = "Redirection";
  } else if (400 <= num && num <= 499) {
    m_class = "Client error";
  } else if (500 <= num && num <= 599) {
    m_class = "Server error";
  } else {
    m_class = "Unknown";
  }
}