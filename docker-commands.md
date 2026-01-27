# Build the image
docker-compose build

# Start the container (in background)
docker-compose up -d

# View logs
docker-compose logs -f

# Stop the container
docker-compose down

# Rebuild and restart
docker-compose up -d --build

# Open shell inside running container
docker-compose exec vector-search bash

# Remove everything (fresh start)
docker-compose down -v
docker system prune -a

---

## First-Time Setup (for new team members)

1. Install Docker Desktop: https://www.docker.com/products/docker-desktop/
2. Clone this repo
3. Run: `docker build -t vector-search .` (takes ~5-7 min first time)
4. Run: `docker-compose up -d`
5. Exit container: Press `Ctrl+C`


## process to test in docker container
1. docker-compose run --rm vector-search bash
2. cd build
3. cmake ..
4. make what ur testing ex.{test_distance}