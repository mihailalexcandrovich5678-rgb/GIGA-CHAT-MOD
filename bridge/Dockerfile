FROM node:24-alpine
WORKDIR /app
COPY bridge/package.json ./package.json
COPY bridge/server.js ./server.js
ENV NODE_ENV=production
EXPOSE 8787
CMD ["npm", "start"]
