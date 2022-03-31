#!/bin/bash

# TODO@Students: Adjust these parameters to suit your needs
GROUP="group-215"
REPOSITORY="openmp"

echo "Now setting up your repository..."
PRIVATE_KEY_FILE="$(pwd)/.ssh/id_rsa"

# This is your repository access key. Don't share this or your repository will be accessible from the outside.
mkdir -p "$(pwd)/.ssh"
cat <<- EOF > "$PRIVATE_KEY_FILE"
-----BEGIN OPENSSH PRIVATE KEY-----
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
-----END OPENSSH PRIVATE KEY-----

EOF

chmod 600 "$PRIVATE_KEY_FILE"

export GIT_SSH_COMMAND="ssh -i '$PRIVATE_KEY_FILE' -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no"

echo "Cloning your repository..."
git clone "git@gitlab.lrz.de:lrr-tum/teaching/parprog/ss2021/project-material/${GROUP}/${REPOSITORY}.git" || exit 255

# Link the data directory. This is only needed for project 1.
if [ -e "${REPOSITORY}/data/" ]
then
  ln -s "$(pwd)/data/COAUTHORCS" "${REPOSITORY}/data/COAUTHORCS" || exit 255
fi

pushd "${REPOSITORY}" || exit 255

git config user.name "ParProg Submission Server"
git config user.email "noreply@parprog.caps.in.tum.de"

echo -e "\nNow running your tasks."
# You now have full access to your repository
# If you create large amounts of data, please remember to remove it to free up storage space from the submission server.

echo "The current directory is $(pwd). Directory listing:"
ls -la

# TODO@Students: Put your run commands here. Adjust these to make them work for your project.
make sequential
perf stat ./sequential

# TODO@Students If you need to, you can commit and push your changes/measurements by uncommenting the following lines:
# git add .
# git commit -m "ParProg Server Test"
# git push

popd || exit 255