
CREATE TABLE patient
(
	name                text CONSTRAINT patient_pk PRIMARY KEY,
	reasons             text DEFAULT '',
	antecedents         text DEFAULT '',
	exams               text DEFAULT '',
	reports             text DEFAULT '',
	birthdate           date DEFAULT NULL,
	address             text DEFAULT '',
	district            text DEFAULT '',
	city                text DEFAULT '',
	state               text DEFAULT '',
	phone1              text DEFAULT '',
	phone2              text DEFAULT '',
	phone3              text DEFAULT '',
	email               text DEFAULT '',
	mother              text DEFAULT '',
	mother_occupation   text DEFAULT '',
	father              text DEFAULT '',
	father_occupation   text DEFAULT '',
	recommendation      text DEFAULT '',
	notes               text DEFAULT ''
);


CREATE TABLE appointment
(
	patient     text
		CONSTRAINT appointment_fk_patient
		REFERENCES patient (name)
		ON DELETE CASCADE
		ON UPDATE CASCADE,
	day         timestamp,
	content     text        DEFAULT '',
	height      real        DEFAULT NULL,
	weight      real        DEFAULT NULL,
	CONSTRAINT appointment_pk
		PRIMARY KEY (patient, day)
);


CREATE INDEX day_idx
	ON appointment (date(day));


CREATE OR REPLACE FUNCTION get_text_default(p_table TEXT, p_column TEXT)
	RETURNS text
	LANGUAGE plpgsql
	AS
		$$
		DECLARE
			casted_default text;
			default_value text;
		BEGIN
			EXECUTE
				'SELECT col.column_default
					FROM information_schema.columns col
					WHERE
						col.table_name = ' || quote_literal(p_table) ||
						' AND col.column_name = ' || quote_literal(p_column)
				INTO casted_default;

			EXECUTE
				'SELECT ' || casted_default
				INTO default_value;

			RETURN default_value;
		END;
		$$;
