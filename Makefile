db-start:
	@docker compose up -d --build
db-down:
	@docker compose down
db-connect:
	@psql -h localhost -p 5432 -U example_user -d journal
db-clear:
	@docker compose down
	@sudo rm -rf data
	@docker compose up -d --build