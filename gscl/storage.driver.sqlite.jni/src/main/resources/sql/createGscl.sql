
-- THERE MUST BE A SEMICOLON AFTER EACH DML STATEMENT
 
-- DROP TABLE ATTRIBUTE;
-- DROP TABLE DOCUMENT;
-- DROP TABLE CONTEXT;
-- DROP TABLE VERSION;

CREATE TABLE VERSION (
    MAJOR character varying(2) NOT NULL,
    MINOR character varying(2) NOT NULL,
    PRIMARY KEY (MAJOR, MINOR)
);

INSERT INTO VERSION (MAJOR, MINOR) VALUES('03', '00');

-- No autoincrement in primary key to improve perfs: http://www.sqlite.org/autoinc.html
CREATE TABLE DOCUMENT (
    ID integer PRIMARY KEY,
    PATH character varying(128) NOT NULL,
    NAME character varying(32) NOT NULL,
    CONTENT binary(4096) NOT NULL
);

CREATE TABLE ATTRIBUTE (
    DOCUMENT_ID integer NOT NULL REFERENCES DOCUMENT ( ID ),
    NAME character varying(32) NOT NULL,
    STRING_VALUE text,
    INT_VALUE integer,
    DATE_VALUE character varying(32),
    ATTRIBUTE_TYPE integer
);

-- Index on document PATH + NAME
CREATE UNIQUE INDEX DOCUMENT_PATH_NAME_IDX ON DOCUMENT ( 
    PATH,
    NAME 
);

-- Index attribute DOCUMENT_ID foreign key
CREATE INDEX ATTRIBUTE_DOCUMENT_ID_IDX ON ATTRIBUTE ( 
    DOCUMENT_ID
);
