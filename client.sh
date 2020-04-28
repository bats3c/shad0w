baseurl="https://172.17.0.2:443/"

function register {
  regreq="username=batman&domain=PWNME&machine=batcave"
  reguri="register"

  curl --insecure --data $regreq "$baseurl$reguri"
}

function check_for_task() {
  taskuri="tasks"

  curl --insecure --data "{\"id\":\"$1\"}" "$baseurl$taskuri"
}

if [ "$1" = "reg" ]; then
  register
fi

if [ "$1" = "id" ]; then
  check_for_task $2
fi

echo -e "\n"
